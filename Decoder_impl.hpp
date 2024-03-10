// Copyright 2021 Rainer Schoenberger
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <inttypes.h>
#include <cstring>
#include <string.h>
#include "Decoder.hpp"

namespace ZCMessagePack
{
template<class T>
GenericDecoder<T> GenericDecoder<T>::operator[](const char * f_mapKey) const
{
    GenericDecoder newGenericDecoder = *this;
    newGenericDecoder.seekElementByKey(f_mapKey);
    return newGenericDecoder;
}

template<class T>
GenericDecoder<T> GenericDecoder<T>::accessArray(uint8_t f_index) const
{
    GenericDecoder<T> newGenericDecoder = *this;
    newGenericDecoder.seekElementByIndex(f_index);
    return newGenericDecoder;
}

template<class T>
void GenericDecoder<T>::seekElementByIndex(uint8_t f_index)
{
    if(not m_validSeek)
    {
        return;
    }

    HeaderInfo header = decodeHeader();
    if(header.headerType != HeaderInfo::Array)
    {
        m_validSeek = false;
        return;
    }
    if(header.numPayloadElements <= f_index)
    {
        m_validSeek = false;
        return;
    }

    m_position += header.headerSize;

    for(uint16_t elementNumber = 0; elementNumber < f_index; elementNumber++)
    {
        seekNextElement();
    }
    return;
}

template<class T>
GenericDecoder<T> GenericDecoder<T>::getMapEntryByIndex(uint8_t f_index, char * f_out_key, uint8_t f_maxSize)
{
    GenericDecoder<T> newGenericDecoder = *this;
    newGenericDecoder.seekMapEntryByIndex(f_index);
    auto len = newGenericDecoder.getString(f_out_key, f_maxSize);
    if(not len.isValid())
    {
        newGenericDecoder.m_validSeek = false;
        return newGenericDecoder;
    }
    newGenericDecoder.seekNextElement();
    return newGenericDecoder;
}

template<class T>
void GenericDecoder<T>::seekMapEntryByIndex(uint8_t f_index)
{
    if(not m_validSeek)
    {
        return;
    }

    HeaderInfo header = decodeHeader();
    if(header.headerType != HeaderInfo::Map)
    {
        m_validSeek = false;
        return;
    }

    if(f_index >= header.numPayloadElements)
    {
        m_validSeek = false;
        return;
    }

    m_position += header.headerSize;

    for(uint16_t elementNumber = 0; elementNumber < f_index; elementNumber++)
    {
            seekNextElement();
            seekNextElement();
    }
    return;
}

template<class T>
Maybe<uint8_t> GenericDecoder<T>::getMapSize() const
{
    if(not m_validSeek)
    {
        return Maybe<uint8_t>();
    }

    HeaderInfo header = decodeHeader();
    if(header.headerType != HeaderInfo::Map)
    {
        return Maybe<uint8_t>();
    }
    return Maybe<uint8_t>(header.numPayloadElements);
}

template<class T>
Maybe<uint8_t> GenericDecoder<T>::getArraySize() const
{
    if(not m_validSeek)
    {
        return Maybe<uint8_t>();
    }

    HeaderInfo header = decodeHeader();
    if(header.headerType != HeaderInfo::Array)
    {
        return Maybe<uint8_t>();
    }
    return Maybe<uint8_t>(header.numPayloadElements);
}

template<class T>
void GenericDecoder<T>::seekElementByKey(const char * f_key)
{
    if(not m_validSeek)
    {
        return;
    }

    HeaderInfo header = decodeHeader();
    if(header.headerType != HeaderInfo::Map)
    {
        m_validSeek = false;
        return;
    }

    m_position += header.headerSize;

    for(uint16_t elementNumber = 0; elementNumber < header.numPayloadElements; elementNumber++)
    {
        auto maybeKeyMatch = compareString(f_key);
        if(not maybeKeyMatch.isValid())
        {
            // key could not be decoded...
            m_validSeek = false;
            return;
        }
        if(maybeKeyMatch.get())
        {
            // key is a match, skip it to reach payload:
            seekNextElement();
            return;
        }
        else
        {
            // key does not match, skip key and value:
            seekNextElement();
            seekNextElement();
        }
    }
    m_validSeek = false;
    return;
}

template<class T>
void GenericDecoder<T>::seekNextElement()
{
    HeaderInfo header = decodeHeader();
    switch(header.headerType)
    {
        case HeaderInfo::InvalidHeader:
            m_validSeek = false;
            return;
        case HeaderInfo::Map:
            m_position += header.headerSize;
            for(uint16_t elementNumber = 0; elementNumber < header.numPayloadElements; elementNumber++)
            {
                uint32_t startpos = m_position;
                seekNextElement();
                seekNextElement();
            }
            return;
        case HeaderInfo::Array:
            m_position += header.headerSize;
            for(uint16_t elementNumber = 0; elementNumber < header.numPayloadElements; elementNumber++)
            {
                seekNextElement();
            }
            return;
        default:
            m_position += header.headerSize + header.numPayloadElements;
    }

    if(m_position >= m_messageSize)
    {
        m_validSeek = false;
    }
}


template<class T>
typename GenericDecoder<T>::HeaderInfo GenericDecoder<T>::decodeHeader() const
{
    HeaderInfo newHeaderInfo;
    if(m_position >= m_messageSize)
    {
        return newHeaderInfo;
    }
    uint8_t typeCode = readRawByte(m_position);

    newHeaderInfo.headerSize = 1;
    newHeaderInfo.numPayloadElements = 0;

    // compressed headers:
    if((typeCode & 0x80) == 0)
    {
        newHeaderInfo.headerType = HeaderInfo::Uint;
        return newHeaderInfo;
    }
    if((typeCode & 0xe0) == 0xe0)
    {
        newHeaderInfo.headerType = HeaderInfo::Int;
        return newHeaderInfo;
    }
    if((typeCode & 0xe0) == 0xa0)
    {
        newHeaderInfo.headerType = HeaderInfo::String;
        newHeaderInfo.numPayloadElements = typeCode & 0x1f;
        return newHeaderInfo;
    }
    switch(typeCode & 0xf0)
    {
        case 0x90:
            newHeaderInfo.headerType = HeaderInfo::Array;
            newHeaderInfo.numPayloadElements = typeCode & 0x0f;
            return newHeaderInfo;
        case 0x80:
            newHeaderInfo.headerType = HeaderInfo::Map;
            newHeaderInfo.numPayloadElements = typeCode & 0x0f;
            return newHeaderInfo;
    }

    // full headers:
    switch(typeCode)
    {
        case 0xc0:
            newHeaderInfo.headerType = HeaderInfo::Nil;
            return newHeaderInfo;
        case 0xc2:
            newHeaderInfo.headerType = HeaderInfo::False;
            return newHeaderInfo;
        case 0xc3:
            newHeaderInfo.headerType = HeaderInfo::True;
            return newHeaderInfo;

        case 0xcc:
            newHeaderInfo.headerType = HeaderInfo::Uint;
            newHeaderInfo.numPayloadElements = 1;
            return newHeaderInfo;
        case 0xcd:
            newHeaderInfo.headerType = HeaderInfo::Uint;
            newHeaderInfo.numPayloadElements = 2;
            return newHeaderInfo;
        case 0xce:
            newHeaderInfo.headerType = HeaderInfo::Uint;
            newHeaderInfo.numPayloadElements = 4;
            return newHeaderInfo;

        case 0xd0:
            newHeaderInfo.headerType = HeaderInfo::Int;
            newHeaderInfo.numPayloadElements = 1;
            return newHeaderInfo;
        case 0xd1:
            newHeaderInfo.headerType = HeaderInfo::Int;
            newHeaderInfo.numPayloadElements = 2;
            return newHeaderInfo;
        case 0xd2:
            newHeaderInfo.headerType = HeaderInfo::Int;
            newHeaderInfo.numPayloadElements = 4;
            return newHeaderInfo;

        case 0xd9:
        case 0xc4:
            if(m_messageSize - m_position < 2)
            {
                return newHeaderInfo;
            }
            newHeaderInfo.headerType = HeaderInfo::String;
            newHeaderInfo.headerSize = 2;
            newHeaderInfo.numPayloadElements = readRawByte(m_position + 1);
            return newHeaderInfo;
        case 0xdc:
            if(m_messageSize - m_position < 3)
            {
                return newHeaderInfo;
            }
            newHeaderInfo.headerType = HeaderInfo::Array;
            newHeaderInfo.headerSize = 3;
            newHeaderInfo.numPayloadElements = (static_cast<uint16_t>(readRawByte(m_position + 1)) << 8) | readRawByte(m_position + 2);
            return newHeaderInfo;
        case 0xde:
            if(m_messageSize - m_position < 3)
            {
                return newHeaderInfo;
            }
            newHeaderInfo.headerType = HeaderInfo::Map;
            newHeaderInfo.headerSize = 3;
            newHeaderInfo.numPayloadElements = (static_cast<uint16_t>(readRawByte(m_position + 1)) << 8) | readRawByte(m_position + 2);
            return newHeaderInfo;
    }

    return newHeaderInfo;
}


template<class T>
Maybe<bool> GenericDecoder<T>::isNil() const
{
    HeaderInfo header = decodeHeader();
    if(header.headerType == HeaderInfo::Nil)
    {
        return Maybe<bool>(true);
    }
    else if(header.headerType != HeaderInfo::InvalidHeader)
    {
        return Maybe<bool>(false);
    }
    else
    {
        // decode failure
        return Maybe<bool>();
    }
}

template<class T>
Maybe<bool> GenericDecoder<T>::getBool() const
{
    HeaderInfo header = decodeHeader();
    if(header.headerType == HeaderInfo::True)
    {
        return Maybe<bool>(true);
    }
    else if(header.headerType == HeaderInfo::False)
    {
        return Maybe<bool>(false);
    }
    else
    {
        // type mismatch
        return Maybe<bool>();
    }
}

template<class T>
Maybe<uint32_t> GenericDecoder<T>::getUint32() const
{
    HeaderInfo header = decodeHeader();
    if(
            header.headerType != HeaderInfo::Uint
            or
            m_position + header.headerSize + header.numPayloadElements > m_messageSize
      )
    {
        // type mismatch
        return Maybe<uint32_t>();
    }
    switch(header.numPayloadElements)
    {
        case 0:
            return Maybe<uint32_t>(readRawByte(m_position) & 0x7f);
        case 1:
            return Maybe<uint32_t>(readRawByte(m_position + 1));
        case 2:
            return Maybe<uint32_t>(static_cast<uint32_t>(readRawByte(m_position + 1)) << 8 | readRawByte(m_position + 2));
        case 4:
            return Maybe<uint32_t>(static_cast<uint32_t>(readRawByte(m_position + 1)) << 24 | static_cast<uint32_t>(readRawByte(m_position + 2)) << 16 | static_cast<uint32_t>(readRawByte(m_position + 3)) << 8 | readRawByte(m_position + 4));
        default:
            // number too big
            return Maybe<uint32_t>();
    }
}

template<class T>
Maybe<uint8_t> GenericDecoder<T>::getUint8() const
{
    auto u32val = getUint32();
    if((not u32val.isValid()) or u32val.get() > 0xff)
    {
        return Maybe<uint8_t>();
    }
    else
    {
        return Maybe<uint8_t>(u32val.get());
    }
}

template<class T>
Maybe<uint16_t> GenericDecoder<T>::getUint16() const
{
    auto u32val = getUint32();
    if((not u32val.isValid()) or u32val.get() > 0xffff)
    {
        return Maybe<uint16_t>();
    }
    else
    {
        return Maybe<uint16_t>(u32val.get());
    }
}

template<class T>
Maybe<uint16_t> GenericDecoder<T>::getString(char * f_out_data, uint8_t f_maxSize) const
{
    if(f_maxSize < 1)
    {
        return Maybe<uint16_t>();
    }
    auto numBytes = getBinary(reinterpret_cast<uint8_t*>(f_out_data), f_maxSize - 1);
    if(numBytes.isValid())
    {
        f_out_data[numBytes.get()] = '\0';
    }
    else
    {
        f_out_data[0] = '\0';
    }
    return numBytes;
}

template<class T>
Maybe<bool> GenericDecoder<T>::compareString(const char * f_string) const
{
    HeaderInfo header = decodeHeader();
    if(
            header.headerType != HeaderInfo::String
            or
            m_position + header.headerSize + header.numPayloadElements > m_messageSize
      )
    {
        // type mismatch
        return Maybe<bool>();
    }

    size_t i = 0;
    for(; i < header.numPayloadElements; i++)
    {
        char stored_char = static_cast<char>(readRawByte(m_position + header.headerSize + i));
        if(f_string[i] == '\0' or stored_char != f_string[i])
        {
            return Maybe<bool>(false);
        }
    }
    if(f_string[i] != '\0')
    {
        return Maybe<bool>(false);
    }
    return Maybe<bool>(true);
}

template<class T>
Maybe<uint16_t> GenericDecoder<T>::getBinary(uint8_t * f_out_data, uint8_t f_maxSize) const
{
    HeaderInfo header = decodeHeader();
    if(
            header.headerType != HeaderInfo::String
            or
            m_position + header.headerSize + header.numPayloadElements > m_messageSize
            or
            header.numPayloadElements > f_maxSize
      )
    {
        // type mismatch
        return Maybe<uint16_t>();
    }

    m_raw_message_reader.read(m_position + header.headerSize, header.numPayloadElements, f_out_data);
    return Maybe<uint16_t>(header.numPayloadElements);
}

template<class T>
template<class Writer>
Maybe<uint16_t> GenericDecoder<T>::getBinary(Writer & writer) const
{
    HeaderInfo header = decodeHeader();
    if(
            header.headerType != HeaderInfo::String
            or
            m_position + header.headerSize + header.numPayloadElements > m_messageSize
      )
    {
        // type mismatch
        return Maybe<uint16_t>();
    }

    for(uint16_t i = 0; i < header.numPayloadElements; i++)
    {
        bool success = writer.write(readRawByte(m_position + header.headerSize + i));
        if(not success)
        {
            return Maybe<uint16_t>();
        }
    }
    return Maybe<uint16_t>(header.numPayloadElements);
}


template<class T>
bool GenericDecoder<T>::isValid()
{
    auto header = decodeHeader();
    if(header.headerType == HeaderInfo::InvalidHeader)
    {
        m_validSeek = false;
    }
    return m_validSeek;
}

template<class T>
uint8_t ZCMessagePack::GenericDecoder<T>::readRawByte(uint8_t offset) const
{
    uint8_t result;
    m_raw_message_reader.read(offset, 1, &result);
    return result;
}

}
