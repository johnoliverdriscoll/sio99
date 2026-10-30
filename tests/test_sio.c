#include <string.h>
#include <sio99/sio_buffer_reader.h>
#include <sio99/sio_buffer_writer.h>
#include "test.h"

typedef struct TestContext {
  unsigned calls;
  uint8_t byte;
  uint64_t position;
  int result;
} TestContext;

static int
read_byte(
  void* user,
  uint8_t* out_value
) {
  TestContext* context = user;
  context->calls++;
  if (out_value != NULL) {
    *out_value = context->byte;
  }
  return context->result;
}

static int
peek_byte(
  void* user,
  uint8_t* out_value,
  bool* out_at_end
) {
  TestContext* context = user;
  context->calls++;
  if (out_value != NULL) {
    *out_value = context->byte;
  }
  if (out_at_end != NULL) {
    *out_at_end = true;
  }
  return context->result;
}

static int
tell_position(
  void* user,
  uint64_t* out_position
) {
  TestContext* context = user;
  context->calls++;
  *out_position = context->position;
  return context->result;
}

static int
seek_position(
  void* user,
  uint64_t position
) {
  TestContext* context = user;
  context->calls++;
  context->position = position;
  return context->result;
}

static int
write_byte(
  void* user,
  uint8_t value
) {
  TestContext* context = user;
  context->calls++;
  context->byte = value;
  return context->result;
}

static void
test_dispatch(void) {
  TestContext context = {0};
  SioSeekableReader reader = {
    .reader = {
      .user = &context,
      .read = read_byte,
      .peek = peek_byte,
    },
    .tell = tell_position,
    .seek = seek_position,
  };
  SioWriter writer = {
    .user = &context,
    .write = write_byte,
  };
  const int results[] = {SIO_OK, SIO_ERROR_IO, 123};
  for (size_t i = 0; i < sizeof(results) / sizeof(*results); i++) {
    context.result = results[i];
    context.calls = 0;
    uint8_t byte = 0;
    bool end = false;
    uint64_t position = 0;
    CHECK(sio_writer_write(&writer, 0xff) == results[i]);
    CHECK(context.byte == 0xff);
    CHECK(sio_reader_read((SioReader*) &reader, &byte) == results[i]);
    CHECK(byte == 0xff);
    CHECK(sio_reader_read((SioReader*) &reader, NULL) == results[i]);
    CHECK(sio_reader_peek((SioReader*) &reader, &byte, &end) == results[i]);
    CHECK(byte == 0xff && end);
    CHECK(sio_reader_peek((SioReader*) &reader, NULL, &end) == results[i]);
    CHECK(sio_reader_peek((SioReader*) &reader, &byte, NULL) == results[i]);
    CHECK(sio_seekable_reader_seek(&reader, UINT64_MAX) == results[i]);
    CHECK(context.position == UINT64_MAX);
    CHECK(sio_seekable_reader_tell(&reader, &position) == results[i]);
    CHECK(position == UINT64_MAX);
    CHECK(context.calls == 8);
  }
}

static void
test_buffer_reader(void) {
  const uint8_t data[] = {0, 0x7f, 0x80, 0xff};
  SioBufferReader reader;
  sio_buffer_reader_init(&reader, data, sizeof(data));
  uint64_t position;
  CHECK(sio_seekable_reader_tell((SioSeekableReader*) &reader, &position) == SIO_OK);
  CHECK(position == 0);
  for (size_t i = 0; i < sizeof(data); i++) {
    uint8_t byte = 0;
    bool end = true;
    CHECK(sio_reader_peek((SioReader*) &reader, &byte, &end) == SIO_OK);
    CHECK(!end && byte == data[i]);
    CHECK(sio_reader_peek((SioReader*) &reader, &byte, NULL) == SIO_OK);
    CHECK(byte == data[i]);
    CHECK(sio_seekable_reader_tell((SioSeekableReader*) &reader, &position) == SIO_OK);
    CHECK(position == i);
    CHECK(sio_reader_read((SioReader*) &reader, &byte) == SIO_OK);
    CHECK(byte == data[i]);
  }
  uint8_t byte = 42;
  bool end = false;
  CHECK(sio_reader_peek((SioReader*) &reader, &byte, &end) == SIO_OK);
  CHECK(end && byte == 42);
  CHECK(sio_reader_read((SioReader*) &reader, &byte) == SIO_ERROR_EOF);
  CHECK(byte == 42);
  CHECK(sio_seekable_reader_tell((SioSeekableReader*) &reader, &position) == SIO_OK);
  CHECK(position == sizeof(data));
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, position) == SIO_OK);
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, position + 1) == SIO_ERROR_EOF);
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, UINT64_MAX) == SIO_ERROR_EOF);
  CHECK(sio_seekable_reader_tell((SioSeekableReader*) &reader, &position) == SIO_OK);
  CHECK(position == sizeof(data));
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, 1) == SIO_OK);
  CHECK(sio_reader_read((SioReader*) &reader, NULL) == SIO_OK);
  CHECK(sio_reader_read((SioReader*) &reader, &byte) == SIO_OK);
  CHECK(byte == 0x80);
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, 0) == SIO_OK);
  CHECK(sio_reader_read((SioReader*) &reader, &byte) == SIO_OK && byte == 0);

  sio_buffer_reader_init(&reader, data, 0);
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, 0) == SIO_OK);
  CHECK(sio_reader_peek((SioReader*) &reader, NULL, &end) == SIO_OK && end);
  CHECK(sio_reader_read((SioReader*) &reader, NULL) == SIO_ERROR_EOF);
}

static void
test_buffer_writer(void) {
  uint8_t data[] = {42, 42, 42, 42};
  const uint8_t expected[] = {0, 0xff, 42, 42};
  SioBufferWriter writer;
  sio_buffer_writer_init(&writer, data, 2);
  CHECK(sio_buffer_writer_get_size(&writer) == 0);
  CHECK(sio_writer_write((SioWriter*) &writer, 0) == SIO_OK);
  CHECK(sio_writer_write((SioWriter*) &writer, 0xff) == SIO_OK);
  CHECK(sio_writer_write((SioWriter*) &writer, 1) == SIO_ERROR_EOF);
  CHECK(sio_buffer_writer_get_size(&writer) == 2);
  CHECK(memcmp(data, expected, sizeof(data)) == 0);
  sio_buffer_writer_reset(&writer);
  CHECK(sio_buffer_writer_get_size(&writer) == 0);
  CHECK(memcmp(data, expected, sizeof(data)) == 0);
  CHECK(sio_writer_write((SioWriter*) &writer, 9) == SIO_OK);
  CHECK(data[0] == 9 && data[1] == 0xff);
  sio_buffer_writer_init(&writer, data, 0);
  CHECK(sio_writer_write((SioWriter*) &writer, 1) == SIO_ERROR_EOF);
  CHECK(sio_buffer_writer_get_size(&writer) == 0 && data[0] == 9);
}

int
main(void) {
  test_dispatch();
  test_buffer_reader();
  test_buffer_writer();
  return EXIT_SUCCESS;
}
