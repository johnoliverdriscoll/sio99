#include <stdlib.h>
#include <string.h>
#include <sio99/sio_dynamic_buffer_writer.h>
#include "sio_assert.h"
#include "sio_require.h"
#include "sio_try.h"

static int
sio_dynamic_buffer_writer_set_error(
  SioDynamicBufferWriter* writer,
  int error
) {
  SIO_ASSERT_NE(writer, NULL);

  writer->error = error;

  return error;
}

static int
sio_dynamic_buffer_writer_reserve(
  SioDynamicBufferWriter* writer,
  size_t required_capacity
) {
  SIO_ASSERT_NE(writer, NULL);

  if (writer->capacity >= required_capacity) {
    return SIO_OK;
  }

  size_t new_capacity = writer->capacity;

  if (new_capacity == 0) {
    new_capacity = 8;
  }

  while (new_capacity < required_capacity) {
    SIO_REQUIRE_LTE(
      new_capacity, SIZE_MAX / 2,
      sio_dynamic_buffer_writer_set_error(writer, SIO_ERROR_OVERFLOW)
    );
    new_capacity *= 2;
  }

  void* new_data = realloc(writer->data, new_capacity);

  SIO_REQUIRE_NE(
    new_data, NULL,
    sio_dynamic_buffer_writer_set_error(writer, SIO_ERROR_NO_MEMORY)
  );

  writer->data = new_data;
  writer->capacity = new_capacity;
  return SIO_OK;
}

static int
sio_dynamic_buffer_writer_write(
  void* user,
  uint8_t c
) {
  SioDynamicBufferWriter* writer = user;

  SIO_ASSERT_NE(writer, NULL);
  SIO_ASSERT_LT(writer->position, SIZE_MAX);

  SIO_TRY(sio_dynamic_buffer_writer_reserve(writer, writer->position + 1));

  SIO_ASSERT_NE(writer->data, NULL);

  writer->data[writer->position++] = c;

  return SIO_OK;
}

void
sio_dynamic_buffer_writer_init(
  SioDynamicBufferWriter* out_writer
) {
  SIO_ASSERT_NE(out_writer, NULL);

  *out_writer = (SioDynamicBufferWriter) {
    .writer = {
      .user = out_writer,
      .write = sio_dynamic_buffer_writer_write,
    },
  };
}

void
sio_dynamic_buffer_writer_deinit(
  SioDynamicBufferWriter* writer
) {
  SIO_ASSERT_NE(writer, NULL);

  free(writer->data);

  memset(writer, 0, sizeof(*writer));
}

size_t
sio_dynamic_buffer_writer_get_size(
  const SioDynamicBufferWriter* writer
) {
  SIO_ASSERT_NE(writer, NULL);

  return writer->position;
}

void
sio_dynamic_buffer_writer_copy_bytes(
  const SioDynamicBufferWriter* writer,
  void* buffer
) {
  SIO_ASSERT_NE(writer, NULL);

  if (writer->position > 0) {
    SIO_ASSERT_NE(buffer, NULL);
    memcpy(buffer, writer->data, writer->position);
  }
}

int
sio_dynamic_buffer_writer_copy_cstring(
  const SioDynamicBufferWriter* writer,
  char* buffer
) {
  SIO_ASSERT_NE(writer, NULL);
  SIO_ASSERT_NE(buffer, NULL);

  for (size_t i = 0; i < writer->position; i++) {
    SIO_REQUIRE_NE(writer->data[i], '\0', SIO_ERROR_INVALID);
  }

  if (writer->position > 0) {
    memcpy(buffer, writer->data, writer->position);
  }

  buffer[writer->position] = '\0';

  return SIO_OK;
}

void
sio_dynamic_buffer_writer_reset(
  SioDynamicBufferWriter* writer
) {
  SIO_ASSERT_NE(writer, NULL);

  writer->position = 0;
}

int
sio_dynamic_buffer_writer_get_error(
  SioDynamicBufferWriter* writer
) {
  SIO_ASSERT_NE(writer, NULL);

  return writer->error;
}
