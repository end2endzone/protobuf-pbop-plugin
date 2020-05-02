#ifndef TESTCALCULATOR_H
#define TESTCALCULATOR_H

#include <gtest/gtest.h>

class TestCalculator : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTCALCULATOR_H
