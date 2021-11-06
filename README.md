# ZeroCopyMessagePack

This is a very lightweight implementation for the MessagePack data format.

It provides an encoder as well as an decoder.

This library is mostly intended to embedded and resource constrained devices:

- Ram usage is minimized, as decoding is done on the fly without copying any data.
- No Heap is used

## Example

Encoding:
```C++
uint8_t message[32];

MessagePackEncoder encoder(message, sizeof(message));

encoder.addMap(3);

encoder.addString("hello");
encoder.addString("world");

encoder.addString("answer");
encoder.addUint(42);

encoder.addString("list");
encoder.addArray(2);
encoder.addBool(true);
encoder.addBool(false);

size_t messageSize = encoder.getMessageSize();
```

Decoding:
```C++
MessagePackDecoder decoder(message, messageSize);

char str[32];
auto stringLength = decoder["hello"].getString(str, sizeof(str));
if(stringLength.isValid())
{
    // Decoding succeeded
    std::cout << "hello: " << str << " length = " stringLength.get(); 
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
