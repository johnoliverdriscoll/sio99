#ifndef SIO_REQUIRE_H
#define SIO_REQUIRE_H

#define SIO_REQUIRE(cond, val)                  \
  do {                                          \
    if (!(cond)) {                              \
      return (val);                             \
    }                                           \
  } while (0)

#define SIO_REQUIRE(cond, val)                  \
  do {                                          \
    if (!(cond)) {                              \
      return (val);                             \
    }                                           \
  } while (0)

#define SIO_REQUIRE_EQ(a, b, val)               \
  do {                                          \
    if ((a) != (b)) {                           \
      return (val);                             \
    }                                           \
  } while (0)

#define SIO_REQUIRE_NE(a, b, val)               \
  do {                                          \
    if ((a) == (b)) {                           \
      return (val);                             \
    }                                           \
  } while (0)

#define SIO_REQUIRE_LT(a, b, val)               \
  do {                                          \
    if ((a) >= (b)) {                           \
      return (val);                             \
    }                                           \
  } while (0)

#define SIO_REQUIRE_LTE(a, b, val)              \
  do {                                          \
    if ((a) > (b)) {                            \
      return (val);                             \
    }                                           \
  } while (0)

#define SIO_REQUIRE_GT(a, b, val)               \
  do {                                          \
    if ((a) <= (b)) {                           \
      return (val);                             \
    }                                           \
  } while (0)

#define SIO_REQUIRE_GTE(a, b, val)              \
  do {                                          \
    if ((a) < (b)) {                            \
      return (val);                             \
    }                                           \
  } while (0)

#endif /* SIO_REQUIRE_H */
