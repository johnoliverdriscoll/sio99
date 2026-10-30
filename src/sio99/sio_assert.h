#ifndef SIO_ASSERT_H
#define SIO_ASSERT_H

#include <assert.h>

#define SIO_ASSERT(cond) assert(cond)

#define SIO_ASSERT_EQ(a, b) SIO_ASSERT((a) == (b))

#define SIO_ASSERT_NE(a, b) SIO_ASSERT((a) != (b))

#define SIO_ASSERT_LT(a, b) SIO_ASSERT((a) < (b))

#define SIO_ASSERT_LTE(a, b) SIO_ASSERT((a) <= (b))

#define SIO_ASSERT_GT(a, b) SIO_ASSERT((a) > (b))

#define SIO_ASSERT_GTE(a, b) SIO_ASSERT((a) >= (b))

#endif /* SIO_ASSERT_H */
