#include <limits.h>
#include <sio99/sio_file_reader.h>
#include "sio_assert.h"
#include "sio_require.h"
#include "sio_try.h"

static int
sio_file_reader_set_error(
  SioFileReader* reader,
  int error
) {
  SIO_ASSERT_NE(reader, NULL);

  reader->error = error;

  return error;
}

static int
sio_file_reader_read(
  void* user,
  uint8_t* out_value
){
  SioFileReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);

  int value = fgetc(reader->file);

  if (value == EOF) {
    SIO_REQUIRE_EQ(
      ferror(reader->file), 0,
      sio_file_reader_set_error(reader, SIO_ERROR_IO)
    );
    SIO_TRY(sio_file_reader_set_error(reader, SIO_ERROR_EOF));
  }

  SIO_REQUIRE(
    value >=0 && value < 256,
    sio_file_reader_set_error(reader, SIO_ERROR_IO)
  );

  if (out_value != NULL) {
    *out_value = (uint8_t) value;
  }
  return SIO_OK;
}

static int
sio_file_reader_peek(
  void* user,
  uint8_t* out_value,
  bool* out_at_end
) {
  SioFileReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT(out_value != NULL || out_at_end != NULL);

  int value = fgetc(reader->file);

  if (value == EOF) {
    SIO_REQUIRE_EQ(
      ferror(reader->file), 0,
      sio_file_reader_set_error(reader, SIO_ERROR_IO)
    );
    if (out_at_end != NULL) {
      *out_at_end = true;
    }
    return SIO_OK;
  }

  SIO_REQUIRE_EQ(
    ungetc(value, reader->file),
    value,
    sio_file_reader_set_error(reader, SIO_ERROR_EOF)
  );

  SIO_REQUIRE(
    value >= 0 && value < 256,
    sio_file_reader_set_error(reader, SIO_ERROR_IO)
  );

  if (out_value != NULL) {
    *out_value = (uint8_t) value;
  }
  if (out_at_end != NULL) {
    *out_at_end = false;
  }
  return SIO_OK;
}

static int
sio_file_reader_tell(
  void* user,
  uint64_t* out_position
){
  SioFileReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(out_position, NULL);

  long position = ftell(reader->file);

  SIO_REQUIRE_GTE(
    position, 0,
    sio_file_reader_set_error(reader, SIO_ERROR_IO)
  );

  *out_position = position;
  return SIO_OK;
}

static int
sio_file_reader_seek(
  void* user,
  uint64_t position
){
  SioFileReader* reader = user;

  SIO_ASSERT_NE(reader, NULL);

  SIO_REQUIRE_LTE(
    position, LONG_MAX,
    sio_file_reader_set_error(reader, SIO_ERROR_OVERFLOW)
  );

  SIO_REQUIRE_EQ(
    fseek(reader->file, position, SEEK_SET), 0,
    sio_file_reader_set_error(reader, SIO_ERROR_IO)
  );

  return SIO_OK;
}

void
sio_file_reader_init(
  SioFileReader* out_reader,
  FILE* file
) {
  SIO_ASSERT_NE(out_reader, NULL);

  *out_reader = (SioFileReader) {
    .reader = {
      .reader = {
        .user = out_reader,
        .read = sio_file_reader_read,
        .peek = sio_file_reader_peek,
      },
      .tell = sio_file_reader_tell,
      .seek = sio_file_reader_seek,
    },
    .file = file,
  };
}

int
sio_file_reader_get_error(
  SioFileReader* reader
) {
  SIO_ASSERT_NE(reader, NULL);

  return reader->error;
}
