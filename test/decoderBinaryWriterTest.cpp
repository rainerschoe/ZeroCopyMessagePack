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

#include <catch2/catch_test_macros.hpp>

#include "Decoder.hpp"
#include "Encoder.hpp"

#include <vector>

using namespace ZCMessagePack;

// Define a mock writer class for testing
class MockWriter {
public:
    bool write(uint8_t data) {
        buffer.push_back(data);
        if(buffer.size() == max_size) {
            return false;
        }
        return true;
    }
    std::vector<uint8_t> buffer;
    uint8_t max_size = 0;
};

struct TestFixture {
    uint8_t binary[2] = {56, 67};
    uint8_t encoded_message[4];

    TestFixture() {
        Encoder encoder(encoded_message, sizeof(encoded_message));
        auto result = encoder.addBinary(binary, sizeof(binary));
        REQUIRE(result == true);
        REQUIRE(encoder.getMessageSize() == 4);
    }

    ~TestFixture() {
        // Any cleanup code goes here
    }
};

TEST_CASE_METHOD(TestFixture, "Successful decode", "") {
    Decoder decoder(encoded_message, 4);

    MockWriter writer;
    auto result = decoder.getBinary(writer);

    REQUIRE(result.isValid() == true);
    REQUIRE(result.get() == 2);
    REQUIRE(writer.buffer.size() == 2);
    REQUIRE(writer.buffer[0] == 56);
    REQUIRE(writer.buffer[1] == 67);
}

TEST_CASE_METHOD(TestFixture, "writer cannot write", "") {
    Decoder decoder(encoded_message, 4);

    MockWriter writer;
    writer.max_size = 1;
    auto result = decoder.getBinary(writer);

    REQUIRE(result.isValid() == false);
    REQUIRE(writer.buffer.size() == 1);
}

TEST_CASE_METHOD(TestFixture, "corrupt header", "") {
    encoded_message[0] = 0xC1;
    Decoder decoder(encoded_message, 4);

    MockWriter writer;
    writer.max_size = 1;
    auto result = decoder.getBinary(writer);

    REQUIRE(result.isValid() == false);
    REQUIRE(writer.buffer.size() == 0);
}