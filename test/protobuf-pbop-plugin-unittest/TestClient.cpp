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

#include "TestClient.h"

#include "pbop/PipeConnection.h"

#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

#ifdef _WIN32
//google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
//google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)
__pragma( warning(push) )
__pragma( warning(disable: 4800))
__pragma( warning(disable: 4146))
#endif //_WIN32

#include "TestPerformance.pb.h"
#include "TestPerformance.pbop.pb.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"

using namespace pbop;

void TestClient::SetUp()
{
}

void TestClient::TearDown()
{
}

extern std::string GetPipeNameFromTestName();
extern std::string GetThreadPrintPrefix();

TEST_F(TestClient, testClientConnectionFail)
{
  std::string pipe_name = GetPipeNameFromTestName();

  PipeConnection connection;
  Status s = connection.Connect(pipe_name.c_str());
  
  ASSERT_FALSE (s.Success());
  ASSERT_EQ(STATUS_CODE_PIPE_ERROR, s.GetCode());
}

TEST_F(TestClient, testClientWithInvalidConnection)
{
  std::string pipe_name = GetPipeNameFromTestName();

  PipeConnection * connection = new pbop::PipeConnection();
  Status s = connection->Connect(pipe_name.c_str()); // The connection should fail.

  performance::Foo::Client client(connection);

  performance::BarRequest request;
  performance::BarResponse response;

  // Make a call using the uninitialized connection
  s = client.Bar(request, response);

  ASSERT_FALSE (s.Success());
  ASSERT_EQ(STATUS_CODE_PIPE_ERROR, s.GetCode());
}
