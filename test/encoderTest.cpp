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
#include <algorithm>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "Encoder.hpp"

using namespace ZCMessagepack;

TEST_CASE( "EncodeEmptyMessageBuffer", "" ) {
    uint8_t buf[0];
    Encoder encoder(buf, 0);

    REQUIRE(encoder.getMessageSize() == 0);

}

TEST_CASE( "EncodeNil", "" ) {
  {
    uint8_t buf[0];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addNil();

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addNil();

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0xc0);
  }
}

TEST_CASE( "EncodeBool_true", "" ) {
  {
    uint8_t buf[0];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBool(true);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBool(true);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0xc3);
  }
}
TEST_CASE( "EncodeBool_false", "" ) {
  {
    uint8_t buf[0];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBool(false);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBool(false);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0xc2);
  }
}

//    - "00"                          # 0 ... 127
TEST_CASE( "EncodeNumber_u7", "" ) {
  {
    uint8_t buf[0];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(0);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(0);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0x00);
  }

  {
    uint8_t buf[2];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(1);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0x01);
  }

  {
    uint8_t buf[4];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(127);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0x7f);
  }
}

//    - "cc-80"                       # unsigned int8 128
TEST_CASE( "EncodeNumber_u8", "" ) {
  {
    uint8_t buf[1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(128);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[2];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(128);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 2);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xcc, 0x80}}));
  }

  {
    uint8_t buf[3];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(255);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 2);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xcc, 0xff}}));
  }
}

//    - "cd-01-00"                    # unsigned int16
TEST_CASE( "EncodeNumber_u16", "" ) {
  {
    uint8_t buf[2];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(256);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[3];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(256);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 3);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xcd, 0x01, 0x00}}));
  }

  {
    uint8_t buf[4];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(65535);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 3);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xcd, 0xff, 0xff}}));
  }
}

TEST_CASE( "EncodeNumber_u32", "" ) {
  {
    uint8_t buf[4];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(65536);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[5];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(65536);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 5);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xce, 0x00, 0x01, 0x00, 0x00}}));
  }

  {
    uint8_t buf[6];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(2147483647);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 5);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xce, 0x7f, 0xff, 0xff, 0xff}}));
  }

  {
    uint8_t buf[7];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(2147483648);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 5);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xce, 0x80, 0x00, 0x00, 0x00}}));
  }

  {
    uint8_t buf[8];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(4294967295);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 5);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xce, 0xff, 0xff, 0xff, 0xff}}));
  }
}

TEST_CASE( "EncodeNumber_u64", "[!shouldfail]" ) {
  std::vector<uint8_t> message{{0xcf, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}};
  uint64_t bignum = 4294967296;
  {
    uint8_t buf[0];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(bignum);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addUint(bignum);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

// ----------------------------------------------------------------------------
// "a0"
TEST_CASE( "EncodeString_empty_small", "" ) {
  {
    uint8_t buf[0];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString("");

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString("");

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0xa0);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xa0}}));
  }

  {
    uint8_t buf[4];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString("a");

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 2);
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == (std::vector<uint8_t>{{0xa1, 0x61}}));
  }
}

// string: "1234567890123456789012345678901"
// msgpack:
//   - "bf-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31"
//   - "d9-1f-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31"
TEST_CASE( "EncodeString_small_compact", "" ) {
  const char* string = "1234567890123456789012345678901";
  std::vector<uint8_t> message{{0xbf, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31}};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

// string: "12345678901234567890123456789012"
TEST_CASE( "EncodeString_big_extended", "" ) {
  const char* string = "12345678901234567890123456789012";
  std::vector<uint8_t> message{{0xd9, 0x20, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32}};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}


TEST_CASE( "EncodeString_toolong", "" ) {
  char string[256];
  std::fill_n(string, 255, 'a');
  string[255] = '\0';
  {
    uint8_t buf[sizeof(string)-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

}
// string: "Кириллица"
TEST_CASE( "EncodeString_utf8_russian", "" ) {
  const char* string = "Кириллица";
  std::vector<uint8_t> message{{0xb2, 0xd0, 0x9a, 0xd0, 0xb8, 0xd1, 0x80, 0xd0, 0xb8, 0xd0, 0xbb, 0xd0, 0xbb, 0xd0, 0xb8, 0xd1, 0x86, 0xd0, 0xb0}};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

TEST_CASE( "EncodeString_utf8_chinese", "" ) {
  const char* string = "汉字";
  std::vector<uint8_t> message{{0xa6, 0xe6, 0xb1, 0x89, 0xe5, 0xad, 0x97}};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addString(string);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}
// ----------------------------------------------------------------------------

TEST_CASE( "EncodeMap_Empty_small", "" ) {
  {
    uint8_t buf[0];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(0);

    REQUIRE(result == false);
    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(0);

    REQUIRE(result == true);
    REQUIRE(encoder.getMessageSize() == 1);
    REQUIRE(buf[0] == 0x80);
  }
}

TEST_CASE( "EncodeMap_Mini_small", "" ) {
  std::vector<uint8_t> message{{0x81, 0xa1, 0x61, 0x01}};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addUint(1);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 3);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addUint(1);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

TEST_CASE( "EncodeMap_small", "" ) {
  std::vector<uint8_t> message{{0x81, 0xa1, 0x61, 0xa1, 0x41}};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addString("A");
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 3);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addString("A");
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}
TEST_CASE( "EncodeArray_small_empty", "" ) {
  std::vector<uint8_t> message{{
            0x90
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(0);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(0);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

TEST_CASE( "EncodeArray_small_one", "" ) {
  std::vector<uint8_t> message{{
            0x91, 0x01,
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addUint(1);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 1);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addUint(1);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

TEST_CASE( "EncodeArray_small_string", "" ) {
  std::vector<uint8_t> message{{
            0x91, 0xa1, 0x61
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 1);
  }
  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}


TEST_CASE( "EncodeArray_small", "" ) {
  std::vector<uint8_t> message{{
            0x9f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }};
  uint8_t array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(sizeof(array));
    REQUIRE(result == true);
    for(uint8_t a: array){
      result = encoder.addUint(a);
      if(a < sizeof(array)){
        REQUIRE(result == true);
      } else {
        REQUIRE(result == false);
      }
    }

    REQUIRE(encoder.getMessageSize() == message.size()-1);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    for(uint8_t a: array){
      result = encoder.addUint(a);
      REQUIRE(result == true);
    }

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(buf[0] != message[0]);
    REQUIRE(buf[0] == 0x91);
    REQUIRE(std::vector<uint8_t>(buf+1, buf+encoder.getMessageSize()) ==  std::vector<uint8_t>(message.begin()+1, message.end()));
  }


  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(sizeof(array));
    REQUIRE(result == true);
    for(uint8_t a: array){
      result = encoder.addUint(a);
      REQUIRE(result == true);
    }

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

TEST_CASE( "EncodeArray_big", "" ) {
  std::vector<uint8_t> message{{
            0xdc, 0x00, 0x10, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
        }};
  uint8_t array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(sizeof(array));
    REQUIRE(result == true);
    for(uint8_t a: array){
      result = encoder.addUint(a);
      if(a < sizeof(array)){
        REQUIRE(result == true);
      } else {
        REQUIRE(result == false);
      }
    }

    REQUIRE(encoder.getMessageSize() == message.size()-1);
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    for(uint8_t a: array){
      result = encoder.addUint(a);
      REQUIRE(result == true);
    }

    REQUIRE(encoder.getMessageSize() == message.size()-2);
    REQUIRE(buf[0] != message[0]);
    REQUIRE(buf[0] == 0x91);
    REQUIRE(std::vector<uint8_t>(buf+1, buf+encoder.getMessageSize()) ==  std::vector<uint8_t>(message.begin()+3, message.end()));
  }

  {
    uint8_t buf[message.size()+1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(sizeof(array));
    REQUIRE(result == true);
    for(uint8_t a: array){
      result = encoder.addUint(a);
      REQUIRE(result == true);
    }

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) == message);
  }
}

// ----------------------------------------------------------------------------
// Binary
TEST_CASE( "EncodeBinary_empty", "" ) {
  std::vector<uint8_t> message{{
            0xc4, 0x00
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBinary(0, 0);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBinary(0, 0);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) ==  message);
  }
}

TEST_CASE( "EncodeBinary_size_one", "" ) {
  std::vector<uint8_t> message{{
            0xc4, 0x01, 0x01
        }};
  uint8_t binary[1] = {1};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBinary(binary, sizeof(binary));
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBinary(binary, sizeof(binary));
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) ==  message);
  }
}

TEST_CASE( "EncodeBinary_size_two", "" ) {
  std::vector<uint8_t> message{{
            0xc4, 0x02, 0x00, 0xff
        }};
  uint8_t binary[] = {0, 255};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBinary(binary, sizeof(binary));
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == 0);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addBinary(binary, sizeof(binary));
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) ==  message);
  }
}


// ----------------------------------------------------------------------------
// nested

TEST_CASE( "EncodeArray_of_array", "" ) {
  std::vector<uint8_t> message{{
            0x91, 0x90
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addArray(0);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == message.size()-1);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addArray(0);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) ==  message);
  }
}

TEST_CASE( "EncodeArray_of_map", "" ) {
  std::vector<uint8_t> message{{
            0x91, 0x80
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addMap(0);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == message.size()-1);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addArray(1);
    REQUIRE(result == true);
    result = encoder.addMap(0);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) ==  message);
  }
}

TEST_CASE( "EncodeMap_of_map", "" ) {
  std::vector<uint8_t> message{{
            0x81, 0xa1, 0x61, 0x80
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addMap(0);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == message.size()-1);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addMap(0);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) ==  message);
  }
}

TEST_CASE( "EncodeMap_of_array", "" ) {
  std::vector<uint8_t> message{{
            0x81, 0xa1, 0x61, 0x90
        }};
  {
    uint8_t buf[message.size()-1];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addArray(0);
    REQUIRE(result == false);

    REQUIRE(encoder.getMessageSize() == message.size()-1);
  }

  {
    uint8_t buf[message.size()];
    Encoder encoder(buf, sizeof(buf));

    auto result = encoder.addMap(1);
    REQUIRE(result == true);
    result = encoder.addString("a");
    REQUIRE(result == true);
    result = encoder.addArray(0);
    REQUIRE(result == true);

    REQUIRE(encoder.getMessageSize() == message.size());
    REQUIRE(std::vector<uint8_t>(buf, buf+encoder.getMessageSize()) ==  message);
  }
}
