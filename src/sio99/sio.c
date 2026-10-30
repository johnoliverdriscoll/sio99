#include <stddef.h>
#include <sio99/sio.h>
#include "sio_assert.h"

int
sio_reader_read(
  SioReader* reader,
  uint8_t* out_value
) {
  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(reader->read, NULL);

  return reader->read(reader->user, out_value);
}

int
sio_reader_peek(
  SioReader* reader,
  uint8_t* out_value,
  bool* out_at_end
) {
  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(reader->peek, NULL);

  return reader->peek(reader->user, out_value, out_at_end);
}

int
sio_seekable_reader_tell(
  SioSeekableReader* reader,
  uint64_t* out_position
) {
  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(reader->tell, NULL);

  return reader->tell(reader->reader.user, out_position);
}

int
sio_seekable_reader_seek(
  SioSeekableReader* reader,
  uint64_t position
) {
  SIO_ASSERT_NE(reader, NULL);
  SIO_ASSERT_NE(reader->seek, NULL);

  return reader->seek(reader->reader.user, position);
}

int
sio_writer_write(
  SioWriter* writer,
  uint8_t value
) {
  SIO_ASSERT_NE(writer, NULL);
  SIO_ASSERT_NE(writer->write, NULL);

  return writer->write(writer->user, value);
}
