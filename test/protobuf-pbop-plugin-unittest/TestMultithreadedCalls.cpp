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
extern std::string GetThreadPrintPrefix();

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
  Thread * thread;
  Status status;

  TestMultithreadServer()
  {
    thread = new ThreadBuilder<TestMultithreadServer>(this, &TestMultithreadServer::Run);
  }
  ~TestMultithreadServer()
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

class TestMultithreadClient
{
public:
  std::string pipe_name;
  Thread * thread;
  Status status;

  TestMultithreadClient()
  {
    thread = new ThreadBuilder<TestMultithreadClient>(this, &TestMultithreadClient::Run);
  }
  ~TestMultithreadClient()
  {
    thread->SetInterrupt();
    thread->Join();
    delete thread;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();

    //Create connection to the server
    pbop::PipeConnection * connection = new pbop::PipeConnection();
    status = connection->Connect(pipe_name.c_str());
    if (!status.Success())
    {
      printf("%s: Error in %s(): %d, %s\n", prefix.c_str(), __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }

    multithreaded::FastSlow::Client client(connection);

    multithreaded::SlowRequest request;
    multithreaded::SlowResponse response;

    // Make 10 calls to CallSlow(). This should take ~10 seconds.
    // Stop making calls if we are ask to stop.
    for(size_t i=0; i<10 && !thread->IsInterrupted(); i++)
    {
      status = client.CallSlow(request, response);
      if (!status.Success())
      {
        printf("%s: Error in %s(): %d, %s\n", prefix.c_str(), __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
        return status.GetCode();
      }
    }

    return 0;
  }
};

TEST_F(TestMultithreadedCalls, testBase)
{
  TestMultithreadServer server_object;

  //assign the service implementation to the server
  FastSlowImpl * impl = new FastSlowImpl();
  ASSERT_TRUE(impl != NULL);
  server_object.server.RegisterService(impl);

  server_object.pipe_name = GetPipeNameFromTestName();

  // Start the server thread
  Status s = server_object.thread->Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Allow time for the server to start listening for connections
  while(!server_object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);


  TestMultithreadClient client_object;
  client_object.pipe_name = server_object.pipe_name;

  // Start the client thread
  s = client_object.thread->Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Create a pipe connection to the server
  PipeConnection * connection = new PipeConnection();
  s = connection->Connect(server_object.pipe_name.c_str());
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Create a second client for making calls to the server
  // while client_object is also making calls to the server
  multithreaded::FastSlow::Client client(connection);

  bool multithread_proof = false;

  double start_time_seconds = ra::timing::GetMillisecondsTimer();
  double end_time_seconds = ra::timing::GetMillisecondsTimer();
  double elapsed_time_seconds = end_time_seconds - start_time_seconds;

  // Make calls for 1 seconds at 100ms intervals
  // Stop making calls if we found our concurrent call proof
  int count = 0;
  while(elapsed_time_seconds <= 1.0 && multithread_proof == false)
  {
    count++;

    multithreaded::FastRequest request;
    multithreaded::FastResponse response;
    printf("CallFast() start count=%d\n", count);
    s = client.CallFast(request, response);
    printf("CallFast() end   count=%d\n", count);
    ASSERT_TRUE( s.Success() ) << s.GetDescription();

    // Did we make our call while the server was processing a call to CallSlow()?
    bool slow_call_in_process = response.slow_call_in_process();
    if (slow_call_in_process)
      multithread_proof = true;

    // Wait before trying again
    Sleep(50);

    // Compute elapsed time
    end_time_seconds = ra::timing::GetMillisecondsTimer();
    elapsed_time_seconds = end_time_seconds - start_time_seconds;
  }

  // Wait for the multithread client to complete
  client_object.thread->SetInterrupt();
  client_object.thread->Join();

  // Assert no error found in the client thread
  ASSERT_TRUE( client_object.status.Success() ) << client_object.status.GetDescription();

  // Ready to shutdown the server
  printf("Shutting down server.\n");
  s = server_object.server.Shutdown();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for the Server::Run call to exit
  server_object.thread->Join();

  // Assert no error found in the server thread
  ASSERT_TRUE( server_object.status.Success() ) << server_object.status.GetDescription();

  ASSERT_TRUE( multithread_proof );
}
