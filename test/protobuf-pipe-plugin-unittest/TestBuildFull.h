#ifndef TESTBUILDFULL_H
#define TESTBUILDFULL_H

#include <gtest/gtest.h>

class TestBuildFull : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTBUILDFULL_H
