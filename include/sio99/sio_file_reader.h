#ifndef SIO_FILE_READER_H
#define SIO_FILE_READER_H

#include <stdio.h>
#include <sio99/sio.h>

/**
 * Caller-owned reader borrowing a `FILE` stream. May be cast to `SioReader*`
 * or `SioSeekableReader*`. Keep this object at a stable address while in use
 * because its callback context points back to it. The caller is responsible
 * for closing the stream. The adapter requires no deinitialization.
 */
typedef struct SioFileReader SioFileReader;

/**
 * Initializes `out_reader` to read from the current position of `file`.
 * `file` must be a non-`NULL` stream open for reading in binary mode.
 * Peeking uses `fgetc`/`ungetc`, so seeking is not required. The reader
 * does not rewind or close the stream. `out_reader` must be non-`NULL`.
 * Reads at EOF return `SIO_ERROR_EOF`, while peeks report EOF without error.
 * Stream read errors return `SIO_ERROR_IO`.
 *
 * Position queries and seeking use `ftell` and `fseek` and require a stream
 * supporting those operations. Positions are relative to the start of the
 * stream, not its position at initialization. Seeking beyond `LONG_MAX`
 * returns `SIO_ERROR_OVERFLOW` without accessing the stream. A failed
 * `ftell` or `fseek` returns `SIO_ERROR_IO`. A successful seek clears EOF.
 */
void sio_file_reader_init(
  SioFileReader* out_reader,
  FILE* file
);

/**
 * Retrieve the last error returned from a read operation.
 * Returns `SIO_OK` if the reader has not returned an error
 * since initialization.
 */
int sio_file_reader_get_error(
  SioFileReader* reader
);

/** Adapter state, initialized by `sio_file_reader_init`. */
struct SioFileReader {

  /** Seekable reader interface. */
  SioSeekableReader reader;

  /** Borrowed stream that must remain open while reading. */
  FILE* file;

  /** Last error returned. */
  int error;
};

#endif /* SIO_FILE_READER_H */
