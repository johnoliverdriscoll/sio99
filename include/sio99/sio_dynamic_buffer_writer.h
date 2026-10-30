#ifndef SIO_DYNAMIC_BUFFER_WRITER_H
#define SIO_DYNAMIC_BUFFER_WRITER_H

#include <stddef.h>
#include <sio99/sio.h>

/**
 * Caller-owned writer that owns a buffer grown with `realloc`. Release the
 * buffer with `sio_dynamic_buffer_writer_deinit`, including after errors.
 * May be cast to `SioWriter*`. Keep this object at a stable address while
 * in use because its callback context points back to it. Writes may move
 * the buffer, invalidating previously obtained pointers into it.
 */
typedef struct SioDynamicBufferWriter SioDynamicBufferWriter;

/**
 * Initializes an empty writer without allocating memory. Subsequent writes
 * grow its buffer with `realloc`. `out_writer` must be non-`NULL`.
 * No terminator is added. Writes return `SIO_ERROR_NO_MEMORY` on allocation
 * failure or `SIO_ERROR_OVERFLOW` on capacity arithmetic overflow. Either
 * error leaves existing bytes, written size, and allocation unchanged.
 * Call `sio_dynamic_buffer_writer_deinit` before reinitializing an existing
 * writer that owns an allocation.
 */
void sio_dynamic_buffer_writer_init(
  SioDynamicBufferWriter* out_writer
);

/**
 * Frees the owned buffer and clears `writer`, invalidating pointers into
 * that buffer. Does not free the caller-owned writer object. `writer` must
 * be non-`NULL` and have been initialized or already deinitialized. Repeated
 * calls are harmless. Call `sio_dynamic_buffer_writer_init` before writing
 * again.
 */
void sio_dynamic_buffer_writer_deinit(
  SioDynamicBufferWriter* writer
);

/**
 * Returns the number of bytes written, not the allocation capacity.
 * `writer` must be non-`NULL` and initialized or deinitialized.
 * Does not reset the writer.
 */
size_t sio_dynamic_buffer_writer_get_size(
  const SioDynamicBufferWriter* writer
);

/**
 * Copies all written bytes into caller-owned `buffer`, without a terminator.
 * `writer` must be non-`NULL` and initialized or deinitialized. For nonempty
 * output, `buffer` must be non-`NULL`, have room for the written size, and
 * not overlap the source bytes. For empty output, `buffer` may be `NULL`.
 * Does not modify the writer or transfer ownership of its allocation.
 */
void sio_dynamic_buffer_writer_copy_bytes(
  const SioDynamicBufferWriter* writer,
  void* buffer
);

/**
 * Copies the written bytes into `buffer` and appends a NUL terminator.
 * `writer` must be non-`NULL` and initialized or deinitialized. `buffer`
 * must be non-`NULL`, have room for the written size plus one byte, and not
 * overlap the source bytes. Empty output produces an empty C string.
 * Returns `SIO_ERROR_INVALID` without modifying `buffer` if the written
 * bytes contain NUL, or `SIO_OK` after copying. Does not modify the writer.
 */
int sio_dynamic_buffer_writer_copy_cstring(
  const SioDynamicBufferWriter* writer,
  char* buffer
);

/**
 * Resets the number of bytes written to zero. Subsequent writes start at the
 * beginning of the buffer. Retains the allocation and leaves existing bytes
 * untouched. `writer` must be
 * non-`NULL` and initialized.
 */
void sio_dynamic_buffer_writer_reset(
  SioDynamicBufferWriter* writer
);

/**
 * Retrieve the last error returned from a write operation.
 * Returns `SIO_OK` if the writer has not returned an error
 * since initialization.
 */
int sio_dynamic_buffer_writer_get_error(
  SioDynamicBufferWriter* writer
);

/** Adapter state, initialized by `sio_dynamic_buffer_writer_init`. */
struct SioDynamicBufferWriter {

  /** Byte writer interface. */
  SioWriter writer;

  /** Owned allocation, or `NULL` before allocation or after deinit. */
  uint8_t* data;

  /** Allocated capacity in bytes. */
  size_t capacity;

  /** Number of bytes written. */
  size_t position;

  /** Last error returned. */
  int error;
};

#endif /* SIO_DYNAMIC_BUFFER_WRITER_H */
