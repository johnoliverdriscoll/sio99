# sio99

Composable byte stream readers and writers for C99. Provides callback interfaces
for reading, peeking, seeking, and writing, with buffer and optional file and
heap adapters.

## Building

```sh
git clone https://github.com/johnoliverdriscoll/sio99.git
cmake -S sio99 -B sio99/build
cmake --build sio99/build
ctest --test-dir sio99/build --output-on-failure
```

See [BUILDING.md](BUILDING.md) for integration, installation, and build options.

## Usage

Read from a buffer and copy each byte to a fixed-capacity writer:

```c
#include <sio99/sio_buffer_reader.h>
#include <sio99/sio_buffer_writer.h>

int copy_example(void) {
  const uint8_t input[] = {'h', 'e', 'l', 'l', 'o'};
  uint8_t output[sizeof(input)];
  SioBufferReader reader;
  SioBufferWriter writer;
  sio_buffer_reader_init(&reader, input, sizeof(input));
  sio_buffer_writer_init(&writer, output, sizeof(output));

  for (;;) {
    uint8_t byte;
    int result = sio_reader_read((SioReader*) &reader, &byte);
    if (result == SIO_ERROR_EOF) {
      return SIO_OK;
    }
    if (result != SIO_OK) {
      return result;
    }
    result = sio_writer_write((SioWriter*) &writer, byte);
    if (result != SIO_OK) {
      return result;
    }
  }
}
```

Adapters can be cast to their interface type. Custom implementations supply
callbacks and a borrowed context pointer. Interface calls return callback
results unchanged. See the public headers for ownership and error contracts.
