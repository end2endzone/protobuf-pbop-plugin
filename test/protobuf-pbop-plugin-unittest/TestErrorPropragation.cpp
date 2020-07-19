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

#include "TestErrorPropragation.h"

#include "pbop/Server.h"
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

#include "TestErrorPropragation.pb.h"
#include "TestErrorPropragation.pbop.pb.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"

using namespace pbop;

void TestErrorPropragation::SetUp()
{
}

void TestErrorPropragation::TearDown()
{
}

extern std::string GetPipeNameFromTestName();
extern std::string GetThreadPrintPrefix();

class PropagatorImpl : public propragation::Propagator::Service
{
public:
  PropagatorImpl() {}
  virtual ~PropagatorImpl() {}

  pbop::Status FailCancelled(const propragation::FailCancelledRequest & request, propragation::FailCancelledResponse & response)
  {
    Status status(STATUS_CODE_CANCELLED, "foo");
    return status;
  }

  pbop::Status FailNotImplemented(const propragation::FailNotImplementedRequest & request, propragation::FailNotImplementedResponse & response)
  {
    Status status(STATUS_CODE_NOT_IMPLEMENTED, "bar");
    return status;
  }

  pbop::Status FailInvArg(const propragation::FailInvArgRequest & request, propragation::FailInvArgResponse & response)
  {
    Status status(STATUS_CODE_INVALID_ARGUMENT, "baz");
    return status;
  }

};

class TestPropragationPlainServer
{
public:
  Server server;
  std::string pipe_name;
  Thread * thread;
  Status status;

  TestPropragationPlainServer()
  {
    thread = new ThreadBuilder<TestPropragationPlainServer>(this, &TestPropragationPlainServer::Run);
  }
  ~TestPropragationPlainServer()
  {
    thread->SetInterrupt();
    thread->Join();
    delete thread;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();
    status = Status::OK;

    printf("%s: Starting server in blocking mode.\n", prefix.c_str());
    status = server.Run(pipe_name.c_str());
    if (!status.Success())
    {
      printf("%s: Error in %s(): %d, %s\n", prefix.c_str(), __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }
    printf("%s: Server returned.\n", prefix.c_str());

    return 0;
  }
};

TEST_F(TestErrorPropragation, testBase)
{
  TestPropragationPlainServer object;

  //assign the service implementation to the server
  PropagatorImpl * impl = new PropagatorImpl();
  ASSERT_TRUE(impl != NULL);
  object.server.RegisterService(impl);

  object.pipe_name = GetPipeNameFromTestName();

  // Start the thread
  Status s = object.thread->Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Allow time for the server to start listening for connections
  while(!object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  // Create a pipe connection to the server
  PipeConnection * connection = new PipeConnection();
  s = connection->Connect(object.pipe_name.c_str());
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Create a client for making calls to the server
  propragation::Propagator::Client client(connection);

  // Test error STATUS_CODE_CANCELLED
  {
    propragation::FailCancelledRequest request;
    propragation::FailCancelledResponse response;
    s = client.FailCancelled(request, response);
    ASSERT_EQ( STATUS_CODE_CANCELLED, s.GetCode() );
    ASSERT_EQ( "foo", s.GetDescription() );
  }

  // Test error STATUS_CODE_NOT_IMPLEMENTED
  {
    propragation::FailNotImplementedRequest request;
    propragation::FailNotImplementedResponse response;
    s = client.FailNotImplemented(request, response);
    ASSERT_EQ( STATUS_CODE_NOT_IMPLEMENTED, s.GetCode() );
    ASSERT_EQ( "bar", s.GetDescription() );
  }

  // Test error STATUS_CODE_INVALID_ARGUMENT
  {
    propragation::FailInvArgRequest request;
    propragation::FailInvArgResponse response;
    s = client.FailInvArg(request, response);
    ASSERT_EQ( STATUS_CODE_INVALID_ARGUMENT, s.GetCode() );
    ASSERT_EQ( "baz", s.GetDescription() );
  }

  // Ready to shutdown the server
  printf("Shutting down server.\n");
  s = object.server.Shutdown();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for the thread to complete
  object.thread->Join();

  // Assert no error found in the thread
  ASSERT_TRUE( object.status.Success() ) << object.status.GetDescription();
}
