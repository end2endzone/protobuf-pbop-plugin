#ifndef TESTPLUGINRUN_H
#define TESTPLUGINRUN_H

#include <gtest/gtest.h>

class TestPluginRun : public ::testing::Test
{
public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif //TESTPLUGINRUN_H
