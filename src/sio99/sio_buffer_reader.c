#include <sio99/sio_buffer_reader.h>
#include "sio_assert.h"
#include "sio_require.h"

static int
sio_buffer_reader_set_error(
  SioBufferReader* reader,
  int error
) {
  SIO_ASSERT_NE(reader, NULL);

  reader->error = error;

  return error;
}

static int
sio_buffer_reader_read(
  void* user,
  uint8_t* out_value
) {
  SioBufferReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(reader->data, NULL);

  SIO_REQUIRE_LT(
    reader->position, reader->size,
    sio_buffer_reader_set_error(reader, SIO_ERROR_EOF)
  );

  if (out_value != NULL) {
    *out_value = reader->data[reader->position];
  }

  reader->position++;

  return SIO_OK;
}

static int
sio_buffer_reader_peek(
  void* user,
  uint8_t* out_value,
  bool* out_at_end
) {
  SIO_ASSERT(out_value != NULL || out_at_end != NULL);

  SioBufferReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(reader->data, NULL);

  if (reader->position < reader->size) {
    if (out_value != NULL) {
      *out_value = reader->data[reader->position];
    }
    if (out_at_end != NULL) {
      *out_at_end = false;
    }
  } else if (out_at_end != NULL) {
    *out_at_end = true;
  }

  return SIO_OK;
}

static int
sio_buffer_reader_tell(
  void* user,
  uint64_t* out_position
) {
  SioBufferReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(out_position, NULL);

  *out_position = reader->position;
  return SIO_OK;
}

static int
sio_buffer_reader_seek(
  void* user,
  uint64_t position
) {
  SioBufferReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);

  SIO_REQUIRE_LTE(
    position,
    reader->size,
    sio_buffer_reader_set_error(reader, SIO_ERROR_EOF)
  );

  reader->position = position;

  return SIO_OK;
}

void
sio_buffer_reader_init(
  SioBufferReader* out_reader,
  const void* data,
  size_t size
) {
  SIO_ASSERT_NE(out_reader, NULL);

  *out_reader = (SioBufferReader) {
    .reader = {
      .reader = {
        .user = out_reader,
        .read = sio_buffer_reader_read,
        .peek = sio_buffer_reader_peek,
      },
      .tell = sio_buffer_reader_tell,
      .seek = sio_buffer_reader_seek,
    },
    .data = data,
    .size = size,
  };
}

int
sio_buffer_reader_get_error(
  SioBufferReader* reader
) {
  SIO_ASSERT_NE(reader, NULL);

  return reader->error;
}
