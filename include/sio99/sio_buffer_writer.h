#ifndef SIO_BUFFER_WRITER_H
#define SIO_BUFFER_WRITER_H

#include <stddef.h>
#include <sio99/sio.h>

/**
 * Caller-owned writer over borrowed, fixed-capacity storage.
 * May be cast to `SioWriter*`. Keep this object at a stable address while
 * in use because its callback context points back to it. Uses no allocations
 * and requires no deinitialization.
 */
typedef struct SioBufferWriter SioBufferWriter;

/**
 * Initializes `out_writer` over `capacity` bytes of caller-owned `data`,
 * with zero bytes written. `data` must be non-`NULL` and remain valid while
 * writing, even for capacity zero. `out_writer` must be non-`NULL`.
 * Writes beyond capacity return `SIO_ERROR_EOF` without modifying
 * the buffer or written size. No terminator is added.
 */
void sio_buffer_writer_init(
  SioBufferWriter* out_writer,
  void* data,
  size_t capacity
);

/**
 * Returns the number of bytes written, not the buffer capacity.
 * `out_writer` must be non-`NULL` and initialized. Does not reset the writer.
 */
size_t sio_buffer_writer_get_size(
  SioBufferWriter* out_writer
);

/**
 * Resets the number of bytes written to zero. Subsequent writes start at the
 * beginning of the buffer. Existing bytes are not cleared.
 * `writer` must be non-`NULL` and initialized.
 */
void sio_buffer_writer_reset(
  SioBufferWriter* writer
);

/**
 * Retrieve the last error returned from a write operation.
 * Returns `SIO_OK` if the writer has not returned an error
 * since initialization.
 */
int sio_buffer_writer_get_error(
  SioBufferWriter* writer
);

/** Adapter state, initialized by `sio_buffer_writer_init`. */
struct SioBufferWriter {

  /** Byte writer interface. */
  SioWriter writer;

  /** Borrowed output buffer that is not freed by this writer. */
  uint8_t* data;

  /** Output capacity in bytes. */
  size_t capacity;

  /** Number of bytes written. */
  size_t position;

  /** Last error returned. */
  int error;
};

#endif /* SIO_BUFFER_WRITER_H */
