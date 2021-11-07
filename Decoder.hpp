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
namespace ZCMessagepack
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

class Decoder
{
    public:
        Decoder(const uint8_t * f_borrow_messageBuffer, uint8_t f_messageSize) :
            m_messageBuffer(f_borrow_messageBuffer),
            m_messageSize(f_messageSize)
        {
        }


        //---------------------------------------------------------------------
        /// The following functions navigate the message:
        
        /// Returns a new decoder which is seeked to the given map key.
        /// FIXME: cannot overload operator[] as implicit conversion is performed from int literal to char * whcih makes it ambiguous
        Decoder operator[](const char * f_mapKey) const;

        /// Returns a new decoder which is seeked to the given array index.
        Decoder accessArray(uint8_t f_index) const;

        /// Resets decoder position to the message root element.
        void seekReset()
        {
            m_position = 0;
            m_validSeek = true;
        }

        /// Set decoder position to map element with given key.
        void seekElementByKey(const char * f_key);

        /// Set decoder position to array element with given index.
        void seekElementByIndex(uint8_t f_index);
        //---------------------------------------------------------------------

        //---------------------------------------------------------------------
        /// The following functions inspect data types:

        /// Not yet implemented
        Maybe<uint8_t> getArraySize() const;

        /// Not yet implemented
        Maybe<uint8_t> getMapSize() const;

        /// Not yet implemented
        bool getMapKey(uint8_t f_index, char * f_out_mapKey, uint8_t f_maxSize) const;
        
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

        void seekNextElement();

        const uint8_t * m_messageBuffer;
        uint8_t m_messageSize;
        uint8_t m_position = 0;
        uint8_t m_validSeek = true;
};
}
