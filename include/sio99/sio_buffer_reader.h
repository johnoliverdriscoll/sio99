#ifndef SIO_BUFFER_READER_H
#define SIO_BUFFER_READER_H

#include <stddef.h>
#include <sio99/sio.h>

/**
 * Caller-owned reader over borrowed, immutable bytes. Uses no allocations.
 * May be cast to `SioReader*` or `SioSeekableReader*`. Keep this object at a
 * stable address while in use because its callback context points back to it.
 * No deinitialization is required.
 */
typedef struct SioBufferReader SioBufferReader;

/**
 * Initializes `out_reader` to read `size` bytes from `data`, starting at zero.
 * `data` must be non-`NULL` and remain valid while reading, even for size zero.
 * `out_reader` must be non-`NULL`. Reads at the end return `SIO_ERROR_EOF`,
 * while peeks report EOF without error. Seeking accepts positions from zero
 * through `size`, inclusive. A position beyond `size` returns
 * `SIO_ERROR_EOF` without changing the current position.
 */
void sio_buffer_reader_init(
  SioBufferReader* out_reader,
  const void* data,
  size_t size
);

/**
 * Retrieve the last error returned from a read operation.
 * Returns `SIO_OK` if the reader has not returned an error
 * since initialization.
 */
int sio_buffer_reader_get_error(
  SioBufferReader* reader
);

/** Adapter state, initialized by `sio_buffer_reader_init`. */
struct SioBufferReader {

  /** Seekable reader interface. */
  SioSeekableReader reader;

  /** Borrowed input buffer that is not freed by this reader. */
  const uint8_t* data;

  /** Input length in bytes. */
  size_t size;

  /** Absolute byte offset of the next read. */
  size_t position;

  /** Last error returned. */
  int error;
};

#endif /* SIO_BUFFER_READER_H */
