#ifndef TESTPATH_H
#define TESTPATH_H

#include <gtest/gtest.h>

class TestPath : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTPATH_H
