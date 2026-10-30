#include <sio99/sio_buffer_writer.h>
#include "sio_assert.h"
#include "sio_require.h"

static int
sio_buffer_writer_set_error(
  SioBufferWriter* writer,
  int error
) {
  SIO_ASSERT_NE(writer, NULL);

  writer->error = error;

  return error;
}

static int
sio_buffer_writer_write(
  void* user,
  uint8_t value
) {
  SioBufferWriter* writer = user;

  SIO_ASSERT_NE(writer, NULL);
  SIO_ASSERT_NE(writer->data, NULL);

  SIO_REQUIRE_LT(
    writer->position, writer->capacity,
    sio_buffer_writer_set_error(writer, SIO_ERROR_EOF)
  );

  writer->data[writer->position++] = value;

  return SIO_OK;
}

void
sio_buffer_writer_init(
  SioBufferWriter* out_writer,
  void* data,
  size_t capacity
) {
  SIO_ASSERT_NE(out_writer, NULL);

  *out_writer = (SioBufferWriter) {
    .writer = {
      .user = out_writer,
      .write = sio_buffer_writer_write,
    },
    .data = data,
    .capacity = capacity,
  };
}

size_t
sio_buffer_writer_get_size(
  SioBufferWriter* writer
) {
  SIO_ASSERT_NE(writer, NULL);

  return writer->position;
}

void
sio_buffer_writer_reset(
  SioBufferWriter* writer
) {
  SIO_ASSERT_NE(writer, NULL);

  writer->position = 0;
}

int
sio_buffer_writer_get_error(
  SioBufferWriter* writer
) {
  SIO_ASSERT_NE(writer, NULL);

  return writer->error;
}
