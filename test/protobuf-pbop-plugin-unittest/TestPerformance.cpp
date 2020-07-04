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

#include "TestPerformance.h"
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

#include "TestPerformance.pb.h"
#include "TestPerformance.pbop.pb.h"
#include "pbop/Server.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <Windows.h>
#undef GetMessage

using namespace pbop;

void TestPerformance::SetUp()
{
}

void TestPerformance::TearDown()
{
}

extern std::string GetPipeNameFromTestName();
extern std::string GetErrorDesription(DWORD code);
extern bool IsThreadAlive(HANDLE hThread);
extern void WaitThreadExit(HANDLE hThread);

class FooServiceImpl : public performance::Foo::Service
{
public:
  FooServiceImpl() {}
  virtual ~FooServiceImpl() {}

  pbop::Status Bar(const performance::BarRequest & request, performance::BarResponse & response)
  {
    return pbop::Status::OK;
  }
};

struct PerformanceServerThreadProcParams
{
  Server * server;
  std::string pipe_name;
};

DWORD WINAPI PerformanceServerThreadProc(LPVOID lpvParam)
{
  // Read the thread's parameters
  PerformanceServerThreadProcParams * params = static_cast<PerformanceServerThreadProcParams*>(lpvParam);
  Server * server = params->server;
  std::string pipe_name = params->pipe_name;

  printf("Starting server in blocking mode.\n");
  Status status = server->Run(pipe_name.c_str());
  printf("Server returned.\n");

  return 0;
}

struct PerformanceClientThreadProcParams
{
  std::string pipe_name;
  bool * ready_flag;
  bool completed;
  double runtime_seconds;
  size_t num_calls;
};

DWORD WINAPI PerformanceClientThreadProc(LPVOID lpvParam)
{
  // Read the thread's parameters
  PerformanceClientThreadProcParams * params = static_cast<PerformanceClientThreadProcParams*>(lpvParam);
  params->completed = false;
  params->runtime_seconds = 0.0;

  double start_time_seconds = ra::timing::GetMillisecondsTimer();

  //Create connection to the server and client
  pbop::PipeConnection * connection = new pbop::PipeConnection();
  pbop::Status status = connection->Connect(params->pipe_name.c_str());
  if (!status.Success())
  {
    printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  performance::Foo::Client client(connection);

  performance::BarRequest request;
  performance::BarResponse response;

  //Wait for the start signal
  while(!params->ready_flag)
  {
    ra::timing::Millisleep(0);
  }

  //execute num_calls
  for(size_t i=0; i<params->num_calls; i++)
  {
    status = client.Bar(request, response);
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetMessage().c_str());
      return status.GetCode();
    }
  }

  //compute elapsed time
  double end_time_seconds = ra::timing::GetMillisecondsTimer();
  double elapsed_time_seconds = end_time_seconds - start_time_seconds;
  params->runtime_seconds = elapsed_time_seconds;

  //notify the execution is complete
  params->completed = true;

  return 0;
}

TEST_F(TestPerformance, testCallPerformance)
{
  pbop::Server server;

  //assign the foo service implementation to the server
  FooServiceImpl * impl = new FooServiceImpl();
  ASSERT_TRUE(impl != NULL);
  server.RegisterService(impl);

  PerformanceServerThreadProcParams params;
  params.server = &server;
  params.pipe_name = GetPipeNameFromTestName();

  // Create a thread that will run this server.
  HANDLE hThread = NULL;
  DWORD  dwThreadId = 0;
  hThread = CreateThread(
    NULL,               // no security attribute
    0,                  // default stack size
    PerformanceServerThreadProc,   // thread proc
    &params,            // thread parameter
    0,                  // not suspended
    &dwThreadId);       // returns thread ID
  ASSERT_FALSE(hThread == NULL);
  //CloseHandle(hThread);

  // Allow time for the server to start listening for connections
  while(!server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  //Define a start signal for the threads to start calling the server.
  bool ready_flag = false;

  //Create 8 threads
  static const size_t num_clients = 4;
  static const size_t num_calls = 10000;
  std::vector<PerformanceClientThreadProcParams> client_params;
  for(size_t i=0; i<num_clients; i++)
  {
    PerformanceClientThreadProcParams params;
    params.pipe_name = GetPipeNameFromTestName();
    params.ready_flag = &ready_flag;
    params.completed = false;
    params.runtime_seconds = 0.0;
    params.num_calls = num_calls;
    client_params.push_back(params);
  }

  //Start the threads
  printf("Creating %d threads.\n", num_clients);
  std::vector<HANDLE> client_threads;
  for(size_t i=0; i<num_clients; i++)
  {
    PerformanceClientThreadProcParams & params = client_params[i];

    // Create a client thread that will run this server.
    HANDLE hThread = NULL;
    DWORD  dwThreadId = 0;
    hThread = CreateThread(
      NULL,               // no security attribute
      0,                  // default stack size
      PerformanceClientThreadProc,   // thread proc
      &params,            // thread parameter
      0,                  // not suspended
      &dwThreadId);       // returns thread ID
    ASSERT_FALSE(hThread == NULL);
    //CloseHandle(hThread);

    printf("Thread index=%d, handle=0x%x, id=0x%x created.\n", i, hThread, dwThreadId);

    client_threads.push_back(hThread);
  }

  // Initiate all threads to start calling the server
  printf("Initiating all threads make calls.\n");
  ready_flag = true;

  // Wait for all the threads to complete
  printf("Waiting for all threads to complete.\n");
  for(size_t i=0; i<num_clients; i++)
  {
    PerformanceClientThreadProcParams & params = client_params[i];

    while(!params.completed)
    {
      ra::timing::Millisleep(100);
    }
  }

  // Wait for the client threads to exit
  printf("Waiting for all threads to exit.\n");
  for(size_t i=0; i<num_clients; i++)
  {
    HANDLE hThread = client_threads[i];
    WaitThreadExit(hThread);
  }

  // Ready to shutdown the server
  printf("Shutting down server.\n");
  Status s = server.Shutdown();
  ASSERT_TRUE( s.Success() ) << s.GetMessage();

  // Wait for the shutdown thread to complete
  printf("Waiting for server to exit.\n");
  WaitThreadExit(hThread);

  // Print performance of each threads
  for(size_t i=0; i<num_clients; i++)
  {
    PerformanceClientThreadProcParams & params = client_params[i];

    double calls_per_seconds = double(num_calls) / params.runtime_seconds;
    double seconds_per_call = 1.0 / calls_per_seconds;
    double ms_per_call = seconds_per_call * 1000.0;
    double us_per_call = ms_per_call * 1000.0;

    printf("Thread %02d made %d calls in %f seconds. In average, that makes %.1f calls/second or %.1f microseconds for each call\n", i, num_calls, params.runtime_seconds, calls_per_seconds, us_per_call);
  }
}
