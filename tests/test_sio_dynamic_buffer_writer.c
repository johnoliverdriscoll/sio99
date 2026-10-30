#include <string.h>
#include <sio99/sio_dynamic_buffer_writer.h>
#include "test.h"

int
main(void) {
  SioDynamicBufferWriter writer;
  sio_dynamic_buffer_writer_init(&writer);
  CHECK(writer.data == NULL && writer.capacity == 0);
  CHECK(sio_dynamic_buffer_writer_get_size(&writer) == 0);
  sio_dynamic_buffer_writer_copy_bytes(&writer, NULL);
  char text[4] = {'x', 'x', 'x', 'x'};
  CHECK(sio_dynamic_buffer_writer_copy_cstring(&writer, text) == SIO_OK);
  CHECK(text[0] == '\0' && text[1] == 'x');

  uint8_t expected[4097];
  uint8_t actual[4098];
  memset(actual, 42, sizeof(actual));
  for (size_t i = 0; i < sizeof(expected); i++) {
    expected[i] = (uint8_t) i;
    CHECK(sio_writer_write((SioWriter*) &writer, expected[i]) == SIO_OK);
    CHECK(sio_dynamic_buffer_writer_get_size(&writer) == i + 1);
  }
  sio_dynamic_buffer_writer_copy_bytes(&writer, actual);
  CHECK(memcmp(actual, expected, sizeof(expected)) == 0);
  CHECK(actual[sizeof(expected)] == 42);

  uint8_t* allocation = writer.data;
  size_t capacity = writer.capacity;
  sio_dynamic_buffer_writer_reset(&writer);
  CHECK(sio_dynamic_buffer_writer_get_size(&writer) == 0);
  CHECK(writer.data == allocation && writer.capacity == capacity);
  CHECK(sio_writer_write((SioWriter*) &writer, 'a') == SIO_OK);
  CHECK(sio_writer_write((SioWriter*) &writer, 'b') == SIO_OK);
  CHECK(sio_dynamic_buffer_writer_copy_cstring(&writer, text) == SIO_OK);
  CHECK(memcmp(text, "ab\0x", sizeof(text)) == 0);
  CHECK(sio_dynamic_buffer_writer_get_size(&writer) == 2);

  CHECK(sio_writer_write((SioWriter*) &writer, 0) == SIO_OK);
  CHECK(sio_writer_write((SioWriter*) &writer, 'c') == SIO_OK);
  memset(text, 'x', sizeof(text));
  CHECK(sio_dynamic_buffer_writer_copy_cstring(&writer, text) == SIO_ERROR_INVALID);
  CHECK(memcmp(text, "xxxx", sizeof(text)) == 0);
  CHECK(sio_dynamic_buffer_writer_get_size(&writer) == 4);
  sio_dynamic_buffer_writer_copy_bytes(&writer, text);
  CHECK(memcmp(text, "ab\0c", sizeof(text)) == 0);

  sio_dynamic_buffer_writer_deinit(&writer);
  CHECK(writer.data == NULL && writer.capacity == 0);
  CHECK(sio_dynamic_buffer_writer_get_size(&writer) == 0);
  sio_dynamic_buffer_writer_deinit(&writer);
  sio_dynamic_buffer_writer_init(&writer);
  CHECK(sio_writer_write((SioWriter*) &writer, 0xff) == SIO_OK);
  sio_dynamic_buffer_writer_copy_bytes(&writer, actual);
  CHECK(actual[0] == 0xff);
  sio_dynamic_buffer_writer_deinit(&writer);
  return EXIT_SUCCESS;
}
