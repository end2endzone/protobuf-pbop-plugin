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

#include "TestMultithreadedCalls.h"

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

#include "TestMultithreadedCalls.pb.h"
#include "TestMultithreadedCalls.pbop.pb.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"

using namespace pbop;

void TestMultithreadedCalls::SetUp()
{
}

void TestMultithreadedCalls::TearDown()
{
}

extern std::string GetPipeNameFromTestName();

class FastSlowImpl : public multithreaded::FastSlow::Service
{
private:
  volatile bool slow_call_in_process_;

public:
  FastSlowImpl() : slow_call_in_process_(false) {}
  virtual ~FastSlowImpl() {}

  pbop::Status CallFast(const multithreaded::FastRequest & request, multithreaded::FastResponse & response)
  {
    response.set_slow_call_in_process(slow_call_in_process_);
    return Status::OK;
  }

  pbop::Status CallSlow(const multithreaded::SlowRequest & request, multithreaded::SlowResponse & response)
  {
    slow_call_in_process_ = true;
    Sleep(1000);
    slow_call_in_process_ = false;

    return Status::OK;
  }

};

class TestMultithreadServer
{
public:
  Server server;
  std::string pipe_name;

  TestMultithreadServer() {}
  ~TestMultithreadServer() {}

  DWORD Run()
  {
    printf("Starting server in blocking mode.\n");
    Status status = server.Run(pipe_name.c_str());
    printf("Server returned.\n");

    return 0;
  }
};

class TestMultithreadClient
{
public:
  std::string pipe_name;
  Thread * thread_;

  TestMultithreadClient() :
    thread_(NULL)
  {
    thread_ = new ThreadBuilder<TestMultithreadClient>(this, &TestMultithreadClient::Run);
  }
  ~TestMultithreadClient()
  {
    delete thread_;
  }

  DWORD Run()
  {
    //Create connection to the server
    pbop::PipeConnection * connection = new pbop::PipeConnection();
    pbop::Status status = connection->Connect(pipe_name.c_str());
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }

    multithreaded::FastSlow::Client client(connection);

    multithreaded::SlowRequest request;
    multithreaded::SlowResponse response;

    status = client.CallSlow(request, response);
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }

    return 0;
  }
};

TEST_F(TestMultithreadedCalls, DISABLED_testBase)
{
  TestMultithreadServer server_object;

  //assign the service implementation to the server
  FastSlowImpl * impl = new FastSlowImpl();
  ASSERT_TRUE(impl != NULL);
  server_object.server.RegisterService(impl);

  server_object.pipe_name = GetPipeNameFromTestName();

  ThreadBuilder<TestMultithreadServer> server_thread(&server_object, &TestMultithreadServer::Run);

  // Start the server thread
  Status s = server_thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Allow time for the server to start listening for connections
  while(!server_object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);


  TestMultithreadClient client_object;
  client_object.pipe_name = server_object.pipe_name;

  ThreadBuilder<TestMultithreadClient> client_thread(&client_object, &TestMultithreadClient::Run);

  // Start the client thread
  s = client_thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Create a pipe connection to the server
  PipeConnection * connection = new PipeConnection();
  s = connection->Connect(server_object.pipe_name.c_str());
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Create a client for making calls to the server
  multithreaded::FastSlow::Client client(connection);

  bool multithread_success = false;

  // Make calls for 1 seconds at 100ms intervals
  for(size_t i=0; i<10; i++)
  {
    multithreaded::FastRequest request;
    multithreaded::FastResponse response;
    s = client.CallFast(request, response);
    ASSERT_TRUE( s.Success() ) << s.GetDescription();

    // Did we make our call while the server was processing a call to CallSlow()?
    bool slow_call_in_process = response.slow_call_in_process();
    if (slow_call_in_process)
      multithread_success = true;

    // Wait before trying again
    Sleep(100);
  }

  // Wait for the multithread client to complete
  client_thread.Join();

  // Ready to shutdown the server
  printf("Shutting down server.\n");
  s = server_object.server.Shutdown();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for the shutdown thread to complete
  server_thread.Join();

  ASSERT_TRUE( multithread_success );
}
