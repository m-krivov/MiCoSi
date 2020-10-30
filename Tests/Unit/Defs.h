
#include <gtest/gtest.h>

#define ASSERT_EQ_EPS(val1, val2, eps)        \
ASSERT_TRUE(std::abs((val1) - (val2)) < (eps))
