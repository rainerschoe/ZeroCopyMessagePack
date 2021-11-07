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

#include "Decoder.hpp"
#include <cstring>

namespace ZCMessagePack
{
Decoder Decoder::operator[](const char * f_mapKey) const
{
    Decoder newDecoder = *this;
    newDecoder.seekElementByKey(f_mapKey);
    return newDecoder;
}

Decoder Decoder::accessArray(uint8_t f_index) const
{
    Decoder newDecoder = *this;
    newDecoder.seekElementByIndex(f_index);
    return newDecoder;
}

void Decoder::seekElementByIndex(uint8_t f_index)
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

void Decoder::seekElementByKey(const char * f_key)
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
void Decoder::seekNextElement()
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


Decoder::HeaderInfo Decoder::decodeHeader() const
{
    HeaderInfo newHeaderInfo;
    if(m_position >= m_messageSize)
    {
        return newHeaderInfo;
    }
    uint8_t typeCode = m_messageBuffer[m_position];

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
            newHeaderInfo.numPayloadElements = m_messageBuffer[m_position + 1];
            return newHeaderInfo;
        case 0xdc:
            if(m_messageSize - m_position < 3)
            {
                return newHeaderInfo;
            }
            newHeaderInfo.headerType = HeaderInfo::Array;
            newHeaderInfo.headerSize = 3;
            newHeaderInfo.numPayloadElements = (m_messageBuffer[m_position + 1] << 8) | m_messageBuffer[m_position + 2];
            return newHeaderInfo;
        case 0xde:
            if(m_messageSize - m_position < 3)
            {
                return newHeaderInfo;
            }
            newHeaderInfo.headerType = HeaderInfo::Map;
            newHeaderInfo.headerSize = 3;
            newHeaderInfo.numPayloadElements = (m_messageBuffer[m_position + 1] << 8) | m_messageBuffer[m_position + 2];
            return newHeaderInfo;
    }

    return newHeaderInfo;
}


Maybe<bool> Decoder::isNil() const
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

Maybe<bool> Decoder::getBool() const
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

Maybe<uint32_t> Decoder::getUint32() const
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
            return Maybe<uint32_t>(m_messageBuffer[m_position] & 0x7f);
        case 1:
            return Maybe<uint32_t>(m_messageBuffer[m_position + 1]);
        case 2:
            return Maybe<uint32_t>(m_messageBuffer[m_position + 1] << 8 | m_messageBuffer[m_position + 2]);
        case 4:
            return Maybe<uint32_t>(m_messageBuffer[m_position + 1] << 24 | m_messageBuffer[m_position + 2] << 16 | m_messageBuffer[m_position + 3] << 8 | m_messageBuffer[m_position + 4]);
        default:
            // number too big
            return Maybe<uint32_t>();
    }
}

Maybe<uint8_t> Decoder::getUint8() const
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

Maybe<uint16_t> Decoder::getUint16() const
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

Maybe<uint16_t> Decoder::getString(char * f_out_data, uint8_t f_maxSize) const
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

Maybe<bool> Decoder::compareString(const char * f_string) const
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

    const char * message = reinterpret_cast<const char*>(&m_messageBuffer[m_position + header.headerSize]);
    size_t i = 0;
    for(; i < header.numPayloadElements; i++)
    {
        if(f_string[i] == '\0' or message[i] != f_string[i])
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

Maybe<uint16_t> Decoder::getBinary(uint8_t * f_out_data, uint8_t f_maxSize) const
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

    memcpy(f_out_data, &m_messageBuffer[m_position + header.headerSize], header.numPayloadElements);
    return Maybe<uint16_t>(header.numPayloadElements);
}


bool Decoder::isValid()
{
    auto header = decodeHeader();
    if(header.headerType == HeaderInfo::InvalidHeader)
    {
        m_validSeek = false;
    }
    return m_validSeek;
}
}
