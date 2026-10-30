#ifndef SIO_TRY_H
#define SIO_TRY_H

#include <sio99/sio.h>

#define SIO_TRY(call)                           \
  do {                                          \
    int sio_try_result_ = (call);               \
    if (sio_try_result_ != SIO_OK) {            \
      return sio_try_result_;                   \
    }                                           \
  } while(0)

#endif /* SIO_TRY_H */
