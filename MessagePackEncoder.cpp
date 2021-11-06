#include "MessagePackEncoder.hpp"
#include <string.h>

MessagePackEncoder::MessagePackEncoder(uint8_t * f_out_borrow_messageBuffer, uint8_t f_bufferSize) :
    m_messageBuffer(f_out_borrow_messageBuffer),
    m_bufferSize(f_bufferSize)
{
}

bool MessagePackEncoder::addUint(uint32_t f_number)
{
    if(f_number <= 0x7f)
    {
        if(sizeLeft() < 1)
        {
            return false;
        }
        m_messageBuffer[m_position] = f_number;
        m_position += 1;
    }
    else if(f_number <= 0xff)
    {
        if(sizeLeft() < 2)
        {
            return false;
        }
        m_messageBuffer[m_position] = 0xcc;
        m_messageBuffer[m_position+1] = f_number;
        m_position += 2;
    }
    else if(f_number <= 0xffff)
    {
        if(sizeLeft() < 3)
        {
            return false;
        }
        m_messageBuffer[m_position] = 0xcd;
        m_messageBuffer[m_position+1] = f_number>>8;
        m_messageBuffer[m_position+2] = f_number;
        m_position += 3;
    }
    else
    {
        if(sizeLeft() < 5)
        {
            return false;
        }
        m_messageBuffer[m_position] = 0xce;
        m_messageBuffer[m_position+1] = f_number>>24;
        m_messageBuffer[m_position+2] = f_number>>16;
        m_messageBuffer[m_position+3] = f_number>>8;
        m_messageBuffer[m_position+4] = f_number;
        m_position += 5;
    }
    return true;
}

bool MessagePackEncoder::addString(const char * f_string)
{
    size_t len = strlen(f_string);
    if(len <= 0x1f)
    {
        if(sizeLeft() < len+1)
        {
            return false;
        }
        m_messageBuffer[m_position] = 0xa | len;
        m_position += 1;
        memcpy(&m_messageBuffer[m_position], f_string, len);
        m_position += len;
    }
    else if(len <= 0xff)
    {
        if(sizeLeft() < len+2)
        {
            return false;
        }
        m_messageBuffer[m_position] = 0xd9;
        m_messageBuffer[m_position+1] = len;
        m_position += 2;
        memcpy(&m_messageBuffer[m_position], f_string, len);
        m_position += len;
    }
    else
    {
        return false;
    }
    return true;
}

bool MessagePackEncoder::addBinary(const uint8_t * f_data, uint8_t f_size)
{
    if(f_size <= 0xff)
    {
        if(sizeLeft() < f_size+2)
        {
            return false;
        }
        m_messageBuffer[m_position] = 0xc4;
        m_messageBuffer[m_position+1] = f_size;
        m_position += 2;
        memcpy(&m_messageBuffer[m_position], f_data, f_size);
        m_position += f_size;
    }
    else
    {
        return false;
    }
    return true;
}

bool MessagePackEncoder::addBool(bool f_value)
{
    if(sizeLeft() < 1)
    {
        return false;
    }
    m_messageBuffer[m_position] = f_value ? 0xc3 : 0xc2;
    m_position += 1;
    return true;
}

bool MessagePackEncoder::addNil()
{
    if(sizeLeft() < 1)
    {
        return false;
    }
    m_messageBuffer[m_position] = 0xc0;
    m_position += 1;
    return true;
}

bool MessagePackEncoder::addMap(uint8_t f_numElements)
{
    return addNestedStructure(f_numElements, 0x80, 0xde);
}

bool MessagePackEncoder::addArray(uint8_t f_numElements)
{
    return addNestedStructure(f_numElements, 0x90, 0xdc);
}

uint8_t MessagePackEncoder::getMessageSize() const
{
    return m_position;
}
uint8_t MessagePackEncoder::sizeLeft()
{
    return m_bufferSize - m_position;
}
bool MessagePackEncoder::addNestedStructure(uint8_t f_numElements, uint8_t f_smallPrefix, uint8_t f_bigPrefix)
{
    if(f_numElements <= 0x0f)
    {
        if(sizeLeft() < 1)
        {
            return false;
        }
        m_messageBuffer[m_position] = f_smallPrefix | f_numElements;
        m_position += 1;
    }
    else
    {
        if(sizeLeft() < 3)
        {
            return false;
        }
        m_messageBuffer[m_position] = f_bigPrefix;
        m_messageBuffer[m_position+1] = f_numElements;
        m_messageBuffer[m_position+2] = f_numElements;
        m_position += 3;
    }
    return true;
}
