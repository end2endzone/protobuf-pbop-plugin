#ifndef TESTBUILDQUICK_H
#define TESTBUILDQUICK_H

#include <gtest/gtest.h>

class TestBuildQuick : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTBUILDQUICK_H
