#ifndef SIO_H
#define SIO_H

#include <stdbool.h>
#include <stdint.h>

/** Result codes. Interface functions return callback results unchanged. */
enum {

  /** Operation completed successfully. */
  SIO_OK,

  /** An underlying input or output operation failed. */
  SIO_ERROR_IO,

  /** No byte remains to read, or fixed output storage is full. */
  SIO_ERROR_EOF,

  /** Data cannot be represented in the requested output format. */
  SIO_ERROR_INVALID,

  /** A size or position exceeds the supported range. */
  SIO_ERROR_OVERFLOW,

  /** An allocation failed. */
  SIO_ERROR_NO_MEMORY,
};

/** Caller-defined byte reader with a borrowed callback context. */
typedef struct SioReader SioReader;

/** Byte reader with absolute byte-position queries and seeking. */
typedef struct SioSeekableReader SioSeekableReader;

/** Caller-defined byte writer with a borrowed callback context. */
typedef struct SioWriter SioWriter;

/**
 * Consumes one byte, storing it in `out_value` when non-`NULL`.
 * Passing `NULL` discards the byte. Returns `SIO_OK` on success or
 * `SIO_ERROR_EOF` when no byte remains. Other callback errors pass through.
 * `reader` and its `read` callback must be non-`NULL`.
 */
int sio_reader_read(
  SioReader* reader,
  uint8_t* out_value
);

/**
 * Inspects the next byte without consuming it. On success, writes the byte
 * to `out_value` when present and sets `out_at_end` to false when present.
 * At EOF, returns `SIO_OK`, leaves `out_value` unchanged, and sets
 * `out_at_end` to true when present. Supply `out_at_end` to distinguish EOF
 * from an available byte. At least one output must be non-`NULL`.
 * `reader` and its `peek` callback must be non-`NULL`.
 * Callback errors pass through unchanged.
 */
int sio_reader_peek(
  SioReader* reader,
  uint8_t* out_value,
  bool* out_at_end
);

/**
 * Stores the absolute byte position of the next read in `out_position`.
 * `reader`, its `tell` callback, and `out_position` must be non-`NULL`.
 * Returns the callback result unchanged. Seeking support and position
 * limits depend on the underlying reader.
 */
int sio_seekable_reader_tell(
  SioSeekableReader* reader,
  uint64_t* out_position
);

/**
 * Sets the next read position to the absolute byte offset `position`.
 * The end position is valid for seekable inputs, including zero for an
 * empty input. Support for positions beyond the end is reader-specific.
 * `reader` and its `seek` callback must be non-`NULL`.
 * Returns the callback result unchanged.
 */
int sio_seekable_reader_seek(
  SioSeekableReader* reader,
  uint64_t position
);

/**
 * Writes one byte. `writer` and its `write` callback must be non-`NULL`.
 * Returns `SIO_OK` on success or the callback's error unchanged.
 * Failure behavior and storage ownership depend on the writer.
 */
int sio_writer_write(
  SioWriter* writer,
  uint8_t value
);

/**
 * Reader callbacks implementing the corresponding `sio_reader_*` contracts.
 * Initialize the callbacks before calling their interface functions.
 * The interface neither allocates nor frees the reader or its context.
 */
struct SioReader {

  /** Borrowed context passed unchanged to each callback. May be `NULL`. */
  void* user;

  /** Consumes one byte, optionally storing it in `out_value`. */
  int (*read)(
    void* user,
    uint8_t* out_value
  );

  /** Inspects the next byte and reports EOF without consuming input. */
  int (*peek)(
    void* user,
    uint8_t* out_peeked,
    bool* out_at_end
  );
};

/**
 * Reader with position callbacks. May be cast to `SioReader*`.
 * All callbacks share the context stored in `reader.user`.
 */
struct SioSeekableReader {

  /** Byte reader interface and shared callback context. */
  SioReader reader;

  /** Reports the absolute byte position, following `sio_seekable_reader_tell`. */
  int (*tell)(
    void* user,
    uint64_t* out_position
  );

  /** Sets the absolute byte position, following `sio_seekable_reader_seek`. */
  int (*seek)(
    void* user,
    uint64_t position
  );
};

/**
 * Writer callback implementing the `sio_writer_write` contract.
 * The interface neither allocates nor frees the writer or its context.
 */
struct SioWriter {

  /** Borrowed context passed unchanged to `write`. May be `NULL`. */
  void* user;

  /** Writes one byte and returns `SIO_OK` or an error code. */
  int (*write)(
    void* user,
    uint8_t value
  );
};

#endif /* SIO_H */
