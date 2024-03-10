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

namespace ZCMessagePack
{
template<class T>
class Maybe
{
    public:
        Maybe() = default;
        Maybe(T f_value) : m_valid(true), m_value(f_value) {}

        /// undefined behavior if invalid
        T get()
        {
            return m_value;
        }
        bool isValid()
        {
            return m_valid;
        }
    private:
    bool m_valid = false;
    T m_value;
};

template<class RawMessageReader>
class GenericDecoder
{

    public:
        // Constructs a new decoder that reads message data from the given message
        // reader. The message reader must provide a read function with the following signature:
        // void read(uint8_t f_offset, uint8_t f_size, uint8_t * f_out_buffer) const
        // where f_offset is the offset in the message buffer, f_size is the number of bytes to read, f_out_buffer is the buffer to write the read data to.
        GenericDecoder(RawMessageReader f_raw_message_reader, uint8_t f_messageSize) :
            m_raw_message_reader(f_raw_message_reader),
            m_messageSize(f_messageSize)
        {
        }

        //---------------------------------------------------------------------
        /// The following functions navigate the message:
        
        /// Returns a new decoder which is seeked to the map value, matching given key.
        /// Returned GenericDecoder will refer to the map value (not the key).
        GenericDecoder operator[](const char * f_mapKey) const;

        /// Returns a new decoder which is seeked to the given array index.
        /// If f_index is out of range, the decoder will become invalid.
        /// NOTE: cannot overload operator[] for array access as implicit conversion is performed from int literal to char * whcih makes it ambiguous
        GenericDecoder accessArray(uint8_t f_index) const;

        /// Resets decoder position to the message root element.
        /// This will recover from an invalid decoder state.
        void seekReset()
        {
            m_position = 0;
            m_validSeek = true;
        }

        /// Set decoder position to map element with given key.
        void seekElementByKey(const char * f_key);

        /// Set decoder position to array element with given index.
        /// If f_index is out of range, the decoder will become invalid.
        void seekElementByIndex(uint8_t f_index);

        /// Retrive both the Key and the Value of a map entry at given index.
        /// @param f_index given index of the map entry. If out of range, an
        ///                invalid GenericDecoder is returned
        /// @param f_out_key user-allocated buffer where the key string will be
        ///                  written to.
        ///                  If a string is written (Valid decoder returned)
        ///                  it is always null terminated.
        /// @param f_maxSize Maximum number of bytes to write to f_out_key
        ///                  (including null termination)
        GenericDecoder getMapEntryByIndex(uint8_t f_index, char * f_out_key, uint8_t f_maxSize);

        //---------------------------------------------------------------------

        //---------------------------------------------------------------------
        /// The following functions inspect data types:

        /// Not yet implemented
        Maybe<uint8_t> getArraySize() const;

        /// If decoder refers to a map, return it's number of entries
        /// (number of entries = number of Key-Value-Pairs).
        /// If decoder does not refer to a map, returns invalid Maybe instance.
        Maybe<uint8_t> getMapSize() const;

        /// Check if current seek position points to valid data
        bool isValid();

        //---------------------------------------------------------------------

        //---------------------------------------------------------------------
        /// The following functions access data members:

        /// Decodes current element as a bool.
        /// @returns the boolean value if decoding was successful
        Maybe<bool> getBool() const;

        /// Checks if current element is set to "Nil"
        /// @returns true/false if decoding was successful
        Maybe<bool> isNil() const;

        /// Decodes current element as an uint32_t.
        /// @returns the integer if decoding was successful
        Maybe<uint32_t> getUint32() const;

        /// Decodes current element as an uint8_t.
        /// @returns the integer if decoding was successful
        Maybe<uint8_t> getUint8() const;

        /// Decodes current element as an uint16_t.
        /// @returns the integer if decoding was successful
        Maybe<uint16_t> getUint16() const;

        /// Reads a String from the MessagePack at current seek position.
        /// @param f_out_data buffer to which read string is written. Terminating '\0' is always added
        /// @returns length of the read string if read was successful
        Maybe<uint16_t> getString(char * f_out_data, uint8_t f_maxSize) const;

        /// Compares the string from the MessagePack at current seek position with given string.
        /// @param f_string null terminated string to compare
        /// @returns true if strings match, false if strings do not match,
        ///          invalid if string could not be decoded
        Maybe<bool> compareString(const char * f_string) const;

        /// Reads a Byte buffer from the MessagePack at current seek position.
        /// @param f_out_data buffer to which data is written.
        /// @returns number of bytes read if read was successful
        Maybe<uint16_t> getBinary(uint8_t * f_out_data, uint8_t f_maxSize) const;

        /// Reads a Byte buffer from the MessagePack at current seek position.
        /// @param writer writer instance which will be used to write the binary data to.
        ///               The writer must provide a write function with the following signature:
        ///               bool write(uint8_t data)
        ///               returning false if data could not be written
        ///               it is called for every byte to write
        /// @returns number of bytes read and written to writer if read was successful
        template<class Writer>
        Maybe<uint16_t> getBinary(Writer & writer) const;
        //---------------------------------------------------------------------

    private:
        struct HeaderInfo
        {
            enum HeaderType
            {
                InvalidHeader,
                Map,
                Array,
                Int,
                Uint,
                True,
                False,
                String,
                Nil
            };
            uint8_t headerSize;
            uint16_t numPayloadElements;
            HeaderType headerType = InvalidHeader;
        };

        HeaderInfo decodeHeader() const;

        uint8_t readRawByte(uint8_t offset) const;

        void seekNextElement();

        /// Set decoder position to map element with given index.
        /// Only works if current seek position is at a map, otherwise GenericDecoder
        /// is set to invalid seek.
        /// After successful seek, key can be read first 
        void seekMapEntryByIndex(uint8_t f_index);

        RawMessageReader m_raw_message_reader;
        uint8_t m_messageSize;
        uint8_t m_position = 0;
        uint8_t m_validSeek = true;
};

class MemoryReader
{
    public:
    MemoryReader(const uint8_t * f_messageBuffer) :
        buffer(f_messageBuffer)
    {
    }

    void read(uint8_t f_offset, uint8_t f_size, uint8_t * f_out_buffer ) const
    {
        std::memcpy(f_out_buffer, buffer + f_offset, f_size);
    }
    private:
    const uint8_t * buffer;
};

/// Convenience class for a decoder that reads from a memory buffer
class Decoder : public GenericDecoder<MemoryReader>
{
    public:
    Decoder(const uint8_t * f_borrow_messageBuffer, uint8_t f_messageSize) :
        GenericDecoder<MemoryReader>(MemoryReader(f_borrow_messageBuffer), f_messageSize)
    {
    }
};

}

#include "Decoder_impl.hpp"
