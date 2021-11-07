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
#include <string.h>

namespace ZCMessagepack
{
class MessagePackEncoder
{
    public:
        /// Constructs the encoder.
        /// The given buffer f_out_borrow_messageBuffer will be used to write
        /// encoded data to.
        MessagePackEncoder(uint8_t * f_out_borrow_messageBuffer, uint8_t f_bufferSize);

        /// Encodes an unsigned integer into the buffer.
        bool addUint(uint32_t f_number);

        /// Encodes a string into the buffer.
        /// f_string needs to be null terminated.
        bool addString(const char * f_string);

        /// Encodes given binary data into the buffer.
        bool addBinary(const uint8_t * f_data, uint8_t f_size);

        /// Encodes given boolean value into the buffer.
        bool addBool(bool f_value);

        /// Encodes "Nil" into the buffer.
        bool addNil();

        /// Add a Map header for the given number of elements.
        /// NOTE: The user needs to ensure that the resulting message is well formed.
        ///       After this header the following tuple needs to be added
        ///       f_numElements times:
        ///         - a string
        ///         - any MessagePack element
        bool addMap(uint8_t f_numElements);

        /// Add an Array header for the given number of elements.
        /// NOTE: The user needs to ensure that the resulting message is well formed.
        ///       After this header f_numElements need to be encoded.
        bool addArray(uint8_t f_numElements);

        /// Returns the size of the encoded message
        uint8_t getMessageSize() const;

    private:
        uint8_t sizeLeft();

        bool addNestedStructure(uint8_t f_numElements, uint8_t f_smallPrefix, uint8_t f_bigPrefix);

        uint8_t * m_messageBuffer;
        uint8_t m_bufferSize;
        uint8_t m_position = 0;
};
}
