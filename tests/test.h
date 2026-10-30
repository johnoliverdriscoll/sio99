#ifndef SIO_TEST_H
#define SIO_TEST_H

#include <stdio.h>
#include <stdlib.h>

/* Keep checks active in release builds, including their side effects. */
#define CHECK(condition) \
  do { \
    if (!(condition)) { \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
      exit(EXIT_FAILURE); \
    } \
  } while (0)

#endif /* SIO_TEST_H */
