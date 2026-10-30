#include <limits.h>
#include <sio99/sio_file_reader.h>
#include "test.h"

int
main(void) {
  const uint8_t data[] = {0, 0x7f, 0x80, 0xff};
  FILE* file = tmpfile();
  CHECK(file != NULL);
  CHECK(fwrite(data, 1, sizeof(data), file) == sizeof(data));
  CHECK(fseek(file, 1, SEEK_SET) == 0);

  SioFileReader reader;
  sio_file_reader_init(&reader, file);
  uint64_t position = 0;
  CHECK(sio_seekable_reader_tell((SioSeekableReader*) &reader, &position) == SIO_OK);
  CHECK(position == 1);
  for (size_t i = 1; i < sizeof(data); i++) {
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
  CHECK(sio_reader_read((SioReader*) &reader, &byte) == SIO_ERROR_EOF);
  CHECK(byte == 42);
  CHECK(sio_reader_peek((SioReader*) &reader, &byte, &end) == SIO_OK);
  CHECK(end && byte == 42);
  CHECK(sio_reader_peek((SioReader*) &reader, NULL, &end) == SIO_OK && end);
  CHECK(sio_reader_read((SioReader*) &reader, NULL) == SIO_ERROR_EOF);
  CHECK(sio_seekable_reader_tell((SioSeekableReader*) &reader, &position) == SIO_OK);
  CHECK(position == sizeof(data));
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, position) == SIO_OK);

  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, 0) == SIO_OK);
  CHECK(sio_reader_peek((SioReader*) &reader, &byte, &end) == SIO_OK);
  CHECK(!end && byte == 0);
  CHECK(sio_reader_read((SioReader*) &reader, NULL) == SIO_OK);
  CHECK(sio_reader_read((SioReader*) &reader, &byte) == SIO_OK && byte == 0x7f);
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, (uint64_t) LONG_MAX + 1) == SIO_ERROR_OVERFLOW);
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, UINT64_MAX) == SIO_ERROR_OVERFLOW);
  CHECK(sio_seekable_reader_tell((SioSeekableReader*) &reader, &position) == SIO_OK);
  CHECK(position == 2);
  CHECK(sio_reader_read((SioReader*) &reader, &byte) == SIO_OK && byte == 0x80);
  CHECK(fclose(file) == 0);

  file = tmpfile();
  CHECK(file != NULL);
  sio_file_reader_init(&reader, file);
  CHECK(sio_seekable_reader_seek((SioSeekableReader*) &reader, 0) == SIO_OK);
  CHECK(sio_reader_peek((SioReader*) &reader, NULL, &end) == SIO_OK && end);
  CHECK(sio_reader_read((SioReader*) &reader, NULL) == SIO_ERROR_EOF);
  CHECK(fclose(file) == 0);
  return EXIT_SUCCESS;
}
