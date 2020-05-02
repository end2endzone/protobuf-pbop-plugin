#ifndef TESTPROTOFUNCTIONS_H
#define TESTPROTOFUNCTIONS_H

#include <gtest/gtest.h>

class TestProtoFunctions : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTPROTOFUNCTIONS_H
