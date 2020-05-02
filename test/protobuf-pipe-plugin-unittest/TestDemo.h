#ifndef TESTDEMO_H
#define TESTDEMO_H

#include <gtest/gtest.h>

class TestDemo : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTDEMO_H
