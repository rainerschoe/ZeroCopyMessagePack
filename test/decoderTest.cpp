//#include "catch2/catch.hpp"
#include <catch2/catch_test_macros.hpp>

#include "MessagePackDecoder.hpp"

TEST_CASE( "EmptyMessageBuffer", "" ) {
    uint8_t buf[0];
    MessagePackDecoder decoder(buf, 0);

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

//    - "00"                          # 0 ... 127
TEST_CASE( "Number_0_u7", "" ) {
    std::vector<uint8_t> message{{0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 0);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 0);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 0);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

//    - "cc-00"                       # unsigned int8
TEST_CASE( "Number_0_u8", "" ) {
    std::vector<uint8_t> message{{0xcc, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 0);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 0);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 0);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

//    - "cd-00-00"                    # unsigned int16
TEST_CASE( "Number_0_u16", "" ) {
    std::vector<uint8_t> message{{0xcd, 0x00, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 0);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 0);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 0);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

//    - "ce-00-00-00-00"              # unsigned int32
TEST_CASE( "Number_0_u32", "" ) {
    std::vector<uint8_t> message{{0xce, 0x00, 0x00, 0x00, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 0);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 0);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 0);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

//    - "cc-00"                       # unsigned int8
TEST_CASE( "Number_0_u8_incomplete", "" ) {
    std::vector<uint8_t> message{{0xcc}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

//    - "cd-00-00"                    # unsigned int16
TEST_CASE( "Number_0_u16_incomplete", "" ) {
    std::vector<uint8_t> message{{0xcd, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

//    - "ce-00-00-00-00"              # unsigned int32
TEST_CASE( "Number_0_u32_incomplete", "" ) {
    std::vector<uint8_t> message{{0xce, 0x00, 0x00, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// 	"7f"
TEST_CASE( "Number_127_u7", "" ) {
    std::vector<uint8_t> message{{0x7f}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 127);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 127);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 127);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// 	"cc-7f"
TEST_CASE( "Number_127_u8", "" ) {
    std::vector<uint8_t> message{{0xcc, 0x7f}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 127);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 127);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 127);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// 	"cd-00-7f"
TEST_CASE( "Number_127_u16", "" ) {
    std::vector<uint8_t> message{{0xcd, 0x00, 0x7f}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 127);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 127);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 127);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// 	"ce-00-00-00-7f"
TEST_CASE( "Number_127_u32", "" ) {
    std::vector<uint8_t> message{{0xce, 0x00, 0x00, 0x00, 0x7f}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 127);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 127);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 127);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// "cc-80"
TEST_CASE( "Number_128_u8", "" ) {
    std::vector<uint8_t> message{{0xcc, 0x80}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 128);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 128);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 128);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// "cd-00-80"
TEST_CASE( "Number_128_u16", "" ) {
    std::vector<uint8_t> message{{0xcd, 0x00, 0x80}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 128);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 128);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 128);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// "ce-00-00-00-80"
TEST_CASE( "Number_128_u32", "" ) {
    std::vector<uint8_t> message{{0xce, 0x00, 0x00, 0x00, 0x80}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == true);
    REQUIRE(u8.get() == 128);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 128);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 128);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// "cd-01-00"
TEST_CASE( "Number_256_u16", "" ) {
    std::vector<uint8_t> message{{0xcd, 0x01, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 256);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 256);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// "ce-00-00-01-00"
TEST_CASE( "Number_256_u32", "" ) {
    std::vector<uint8_t> message{{0xce, 0x00, 0x00, 0x01, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == true);
    REQUIRE(u16.get() == 256);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 256);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// "ce-00-01-00-00"
TEST_CASE( "Number_0x010000_u32", "" ) {
    std::vector<uint8_t> message{{0xce, 0x00, 0x01, 0x00, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == true);
    REQUIRE(u32.get() == 0x010000);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);
}

// ----------------------------------------------------------------------------
// "a0"
TEST_CASE( "String_empty_small", "" ) {
    std::vector<uint8_t> message{{0xa0}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[3] = {8,9,10};
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == true);
    REQUIRE(len.get() == 0);
    REQUIRE(str[0] == '\0');

    char str2[1] = {8};
    auto len3 = decoder.getString(str2, sizeof(str2));
    REQUIRE(len3.isValid() == true);
    REQUIRE(len3.get() == 0);
    REQUIRE(str2[0] == '\0');

    char str3[0];
    auto len4 = decoder.getString(str3, sizeof(str3));
    REQUIRE(len4.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == true);
    REQUIRE(len2.get() == 0);

    auto res = decoder.compareString("");
    REQUIRE(res.isValid() == true);
    REQUIRE(res.get() == true);

    auto res2 = decoder.compareString("hello");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
}

// "d9-00"
TEST_CASE( "String_empty_big", "" ) {
    std::vector<uint8_t> message{{0xd9, 0x00}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[3] = {8,9,10};
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == true);
    REQUIRE(len.get() == 0);
    REQUIRE(str[0] == '\0');

    char str2[1] = {8};
    auto len3 = decoder.getString(str2, sizeof(str2));
    REQUIRE(len3.isValid() == true);
    REQUIRE(len3.get() == 0);
    REQUIRE(str2[0] == '\0');

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == true);
    REQUIRE(len2.get() == 0);

    auto res = decoder.compareString("");
    REQUIRE(res.isValid() == true);
    REQUIRE(res.get() == true);

    auto res2 = decoder.compareString("hello");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
}

// "d9-00"
TEST_CASE( "String_empty_big_invalid", "" ) {
    std::vector<uint8_t> message{{0xd9}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[3] = {8,9,10};
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("");
    REQUIRE(res.isValid() == false);

    auto res2 = decoder.compareString("hello");
    REQUIRE(res2.isValid() == false);
}

// string: "1234567890123456789012345678901"
// msgpack:
//   - "bf-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31"
//   - "d9-1f-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31-32-33-34-35-36-37-38-39-30-31"
TEST_CASE( "String_small_compact", "" ) {
    std::vector<uint8_t> message{{0xbf, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[32];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == true);
    REQUIRE(len.get() == 31);
    REQUIRE(str[31] == '\0');
    REQUIRE(std::string(str) == "1234567890123456789012345678901");

    char str3[0];
    auto len4 = decoder.getString(str3, sizeof(str3));
    REQUIRE(len4.isValid() == false);

    uint8_t bin[31];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == true);
    REQUIRE(len2.get() == 31);

    auto res = decoder.compareString("1234567890123456789012345678901");
    REQUIRE(res.isValid() == true);
    REQUIRE(res.get() == true);

    {
    auto res2 = decoder.compareString("hello");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }
    {
    auto res2 = decoder.compareString("");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }
    {
    auto res2 = decoder.compareString("1234567890123456789012345678901 ");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }

}

// string: "1234567890123456789012345678901"
TEST_CASE( "String_small_extended", "" ) {
    std::vector<uint8_t> message{{0xd9, 0x1f, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[32];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == true);
    REQUIRE(len.get() == 31);
    REQUIRE(str[31] == '\0');
    REQUIRE(std::string(str) == "1234567890123456789012345678901");

    char str3[0];
    auto len4 = decoder.getString(str3, sizeof(str3));
    REQUIRE(len4.isValid() == false);

    uint8_t bin[31];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == true);
    REQUIRE(len2.get() == 31);

    auto res = decoder.compareString("1234567890123456789012345678901");
    REQUIRE(res.isValid() == true);
    REQUIRE(res.get() == true);

    {
    auto res2 = decoder.compareString("hello");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }
    {
    auto res2 = decoder.compareString("");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }
    {
    auto res2 = decoder.compareString("1234567890123456789012345678901 ");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }

}

// string: "12345678901234567890123456789012"
TEST_CASE( "String_big_extended", "" ) {
    std::vector<uint8_t> message{{0xd9, 0x20, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[33];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == true);
    REQUIRE(len.get() == 32);
    REQUIRE(str[32] == '\0');
    REQUIRE(std::string(str) == "12345678901234567890123456789012");

    char str3[0];
    auto len4 = decoder.getString(str3, sizeof(str3));
    REQUIRE(len4.isValid() == false);

    uint8_t bin[32];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == true);
    REQUIRE(len2.get() == 32);

    auto res = decoder.compareString("12345678901234567890123456789012");
    REQUIRE(res.isValid() == true);
    REQUIRE(res.get() == true);

    {
    auto res2 = decoder.compareString("hello");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }
    {
    auto res2 = decoder.compareString("");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }
    {
    auto res2 = decoder.compareString("12345678901234567890123456789012 ");
    REQUIRE(res2.isValid() == true);
    REQUIRE(res2.get() == false);
    }

}

// ----------------------------------------------------------------------------

TEST_CASE( "Map_Empty_small", "" ) {
    std::vector<uint8_t> message{{0x80}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);

    REQUIRE(decoder["asd"].isValid() == false);
    REQUIRE(decoder.accessArray(0).isValid() == false);
    REQUIRE(decoder.isValid() == true);
}

TEST_CASE( "Map_Mini_small", "" ) {
    std::vector<uint8_t> message{{0x81, 0xa1, 0x61, 0xa1, 0x41}};

    MessagePackDecoder decoder(message.data(), message.size());

    auto u8 = decoder.getUint8();
    REQUIRE(u8.isValid() == false);

    auto u16 = decoder.getUint16();
    REQUIRE(u16.isValid() == false);

    auto u32 = decoder.getUint32();
    REQUIRE(u32.isValid() == false);

    char str[16];
    auto len = decoder.getString(str, sizeof(str));
    REQUIRE(len.isValid() == false);

    uint8_t bin[16];
    auto len2 = decoder.getBinary(bin, sizeof(bin));
    REQUIRE(len2.isValid() == false);

    auto res = decoder.compareString("hello");
    REQUIRE(res.isValid() == false);

    REQUIRE(decoder["asd"].isValid() == false);
    REQUIRE(decoder.accessArray(0).isValid() == false);
    REQUIRE(decoder[""].isValid() == false);
    REQUIRE(decoder["a"].isValid() == true);
    char mapStr[5];
    auto strlen = decoder["a"].getString(mapStr, sizeof(mapStr));
    REQUIRE(decoder.isValid() == true);
    REQUIRE(strlen.isValid() == true);
    REQUIRE(strlen.get() == 1);
    REQUIRE(std::string(mapStr) == "A");
}



