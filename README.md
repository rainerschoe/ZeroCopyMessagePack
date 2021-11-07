# ZeroCopyMessagePack
[![codecov](https://codecov.io/gh/rainerschoe/ZeroCopyMessagePack/branch/master/graph/badge.svg?token=S235FDPV5E)](https://codecov.io/gh/rainerschoe/ZeroCopyMessagePack)

This is a very lightweight C++ implementation for the MessagePack data format.

It provides an encoder as well as a decoder.

This library is intended for embedded and resource constrained devices:

- Ram usage is minimized, as decoding is done on the fly without copying any data.
- No Heap is used

## Example

Encoding:
```C++
uint8_t message[32];

ZCMessagePack::Encoder encoder(message, sizeof(message));

bool encodeResult = true;

encodeResult &= encoder.addMap(3);

encodeResult &= encoder.addString("hello");
encodeResult &= encoder.addString("world");

encodeResult &= encoder.addString("answer");
encodeResult &= encoder.addUint(42);

encodeResult &= encoder.addString("list");
encodeResult &= encoder.addArray(2);
encodeResult &= encoder.addBool(true);
encodeResult &= encoder.addBool(false);

size_t messageSize = encoder.getMessageSize();
if(encodeResult)
{
    std::cout << "Encode succeeded. MessageSize = " << messageSize;
}
```

Decoding:
```C++
ZCMessagePack::Decoder decoder(message, messageSize);

char str[32];
auto stringLength = decoder["hello"].getString(str, sizeof(str));
if(stringLength.isValid())
{
    // Decoding succeeded
    std::cout << "hello: " << str << " length = " stringLength.get(); 
}

auto answer = decoder["answer"].getUint8();
if(answer.isValid())
{
    // Decoding succeeded
    std::cout << "answer: " << answer.get(); 
}

auto booleanValue = decoder["list"].accessArray(0).getBool();
if(booleanValue.isValid())
{
    // Decoding succeeded
    std::cout << "list[0] = " << booleanValue.get(); 
}

```

## Limitations

- Number of elements in Maps or Arrays is limited to 256
- Number of bytes/chars in binary data or strings is limited to 256
- Floats are not supported
- Decoding nested messages requires ~4 bytes of stack (ram) per nesting level.
  This can be avoided by using the seek functions instead of `operator[]()` or `accessArrayElement()`
