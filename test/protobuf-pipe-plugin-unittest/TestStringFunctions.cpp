#include "TestStringFunctions.h"
#include "stringfunc.h"

void TestStringFunctions::SetUp()
{
}

void TestStringFunctions::TearDown()
{
}

TEST_F(TestStringFunctions, testGetMaxIndexOf)
{
  ASSERT_EQ( (size_t)-1, getMaxIndexOf( (size_t)-1, (size_t)-1 ) );
  ASSERT_EQ( (size_t)3, getMaxIndexOf( (size_t)-1, (size_t)3 ) );
  ASSERT_EQ( (size_t)3, getMaxIndexOf( (size_t)3, (size_t)-1 ) );
  ASSERT_EQ( (size_t)5, getMaxIndexOf( (size_t)3, (size_t)5 ) );
  ASSERT_EQ( (size_t)5, getMaxIndexOf( (size_t)5, (size_t)3 ) );
}

TEST_F(TestStringFunctions, testCapitalize)
{
  struct CAPITALIZE_RESULT
  {
    std::string value;
    std::string expected;
    std::string actual;
  };
  CAPITALIZE_RESULT results[] = {
    {"","",""},
    {"a","A",""},
    {"A","A",""},
    {"foo","FOO",""},
    {"Bar","BAR",""},
    {"TEST","TEST",""},
  };
  size_t numResults = sizeof(results)/sizeof(results[0]);

  for(size_t i=0; i<numResults; i++)
  {
    CAPITALIZE_RESULT & result = results[i];
    result.actual = capitalize(result.value);

    ASSERT_EQ(result.expected, result.actual) /*<< "Using value: '" << result.value.c_str() << "'. "
      << "Received value: '" << result.actual.c_str() << "'. "
      << "while expecting value: '" << result.expected.c_str() << "'. "*/;
  }
}

TEST_F(TestStringFunctions, testCapitalizeFirst)
{
  struct CAPITALIZE_RESULT
  {
    std::string value;
    std::string expected;
    std::string actual;
  };
  CAPITALIZE_RESULT results[] = {
    {"","",""},
    {"a","A",""},
    {"A","A",""},
    {"foo","Foo",""},
    {"Bar","Bar",""},
    {"TEST","TEST",""},
  };
  size_t numResults = sizeof(results)/sizeof(results[0]);

  for(size_t i=0; i<numResults; i++)
  {
    CAPITALIZE_RESULT & result = results[i];
    result.actual = capitalizeFirst(result.value);

    ASSERT_EQ(result.expected, result.actual) /*<< "Using value: '" << result.value.c_str() << "'. "
      << "Received value: '" << result.actual.c_str() << "'. "
      << "while expecting value: '" << result.expected.c_str() << "'. "*/;
  }
}

TEST_F(TestStringFunctions, testStringReplace)
{
  struct REPLACE_RESULT
  {
    std::string str;
    std::string oldValue;
    std::string newValue;
    std::string expected;
    std::string actual;
  };
  REPLACE_RESULT results[] = {
    {"","","","",""},
    {"Foo","F","B","Boo",""},

    //replace Bar in string whereever it is found
    {"Bar","Bar","Jackpot","Jackpot",""},
    {"FooBar","Bar","Jackpot","FooJackpot",""},
    {"BarFoo","Bar","Jackpot","JackpotFoo",""},
    {"FooBarFoo","Bar","Jackpot","FooJackpotFoo",""},

    //remove Bar in string whereever it is found
    {"Bar","Bar","","",""},
    {"FooBar","Bar","","Foo",""},
    {"BarFoo","Bar","","Foo",""},
    {"FooBarFoo","Bar","","FooFoo",""},

    //only old string by new string
    {"FooFooFoo","Foo","Bar","BarBarBar",""},
  };
  size_t numResults = sizeof(results)/sizeof(results[0]);

  for(size_t i=0; i<numResults; i++)
  {
    REPLACE_RESULT & result = results[i];
    result.actual = result.str;
    stringReplace(result.actual, result.oldValue.c_str(), result.newValue.c_str());

    ASSERT_EQ(result.expected, result.actual);
  }
}
