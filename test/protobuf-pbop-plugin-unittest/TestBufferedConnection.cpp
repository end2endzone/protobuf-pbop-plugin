/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "TestBufferedConnection.h"
#include "pbop/BufferedConnection.h"

using namespace pbop;

void TestBufferedConnection::SetUp()
{
}

void TestBufferedConnection::TearDown()
{
}

TEST_F(TestBufferedConnection, testReadWrite)
{
  std::string bufferA;
  std::string bufferB;

  BufferedConnection conn1(&bufferA, &bufferB);
  BufferedConnection conn2(&bufferB, &bufferA);

  Status s;

  //write data to connection 1
  const std::string write_data = "hello!";
  s = conn1.Write(write_data);
  ASSERT_TRUE( s.Success() ) << s.GetMessage();

  //read data from connection 2
  std::string read_data;
  s = conn2.Read(read_data);
  ASSERT_TRUE( s.Success() ) << s.GetMessage();

  //expect readed and written data to be identical.
  ASSERT_EQ(write_data, read_data);
}

TEST_F(TestBufferedConnection, testInvalidWrite)
{
  std::string buffer;

  BufferedConnection conn(&buffer, NULL);

  //write data to connection
  const std::string data = "hello!";
  Status s = conn.Write(data);
  ASSERT_FALSE( s.Success() ) << s.GetMessage();
}

TEST_F(TestBufferedConnection, testInvalidRead)
{
  std::string buffer;

  BufferedConnection conn(NULL, &buffer);

  //read data to connection
  std::string data;
  Status s = conn.Read(data);
  ASSERT_FALSE( s.Success() ) << s.GetMessage();
}
