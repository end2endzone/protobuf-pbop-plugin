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

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"

using namespace pbop;

void TestPerformance::SetUp()
{
}

void TestPerformance::TearDown()
{
}

extern std::string GetPipeNameFromTestName();
extern std::string GetThreadPrintPrefix();

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

class TestPerformanceServer
{
public:
  Server server;
  std::string pipe_name;

  TestPerformanceServer() {}
  ~TestPerformanceServer() {}

  DWORD Run()
  {
    printf("Starting server in blocking mode.\n");
    Status status = server.Run(pipe_name.c_str());
    printf("Server returned.\n");

    return 0;
  }
};

class TestPerformanceClient
{
public:
  std::string pipe_name;
  volatile bool * ready_flag;
  volatile bool completed;
  volatile double runtime_seconds;
  size_t num_calls;
  Thread * thread_;

  TestPerformanceClient() :
    thread_(NULL)
  {
    thread_ = new ThreadBuilder<TestPerformanceClient>(this, &TestPerformanceClient::Run);
  }
  ~TestPerformanceClient()
  {
    delete thread_;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();
    completed = false;
    runtime_seconds = 0.0;

    double start_time_seconds = ra::timing::GetMillisecondsTimer();

    //Create connection to the server and client
    pbop::PipeConnection * connection = new pbop::PipeConnection();
    pbop::Status status = connection->Connect(pipe_name.c_str());
    if (!status.Success())
    {
      printf("%s: Error in %s(): %d, %s\n", prefix.c_str(), __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }

    performance::Foo::Client client(connection);

    performance::BarRequest request;
    performance::BarResponse response;

    //Wait for the start signal
    while(!ready_flag)
    {
      ra::timing::Millisleep(0);
    }

    //execute num_calls
    for(size_t i=0; i<num_calls; i++)
    {
      status = client.Bar(request, response);
      if (!status.Success())
      {
        printf("%s: Error in %s(): %d, %s\n", prefix.c_str(), __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
        return status.GetCode();
      }
    }

    //compute elapsed time
    double end_time_seconds = ra::timing::GetMillisecondsTimer();
    double elapsed_time_seconds = end_time_seconds - start_time_seconds;
    runtime_seconds = elapsed_time_seconds;

    //notify the execution is complete
    completed = true;

    return 0;
  }
};

TEST_F(TestPerformance, testCallPerformance)
{
  TestPerformanceServer object;

  //assign the foo service implementation to the server
  FooServiceImpl * impl = new FooServiceImpl();
  ASSERT_TRUE(impl != NULL);
  object.server.RegisterService(impl);

  object.pipe_name = GetPipeNameFromTestName();

  ThreadBuilder<TestPerformanceServer> thread(&object, &TestPerformanceServer::Run);

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Allow time for the server to start listening for connections
  while(!object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  //Define a start signal for the client threads to start calling the server.
  bool ready_flag = false;

  //Create many threads
  static const size_t num_clients = 4;
  static const size_t num_calls = 10000;
  std::vector<TestPerformanceClient *> clients;
  for(size_t i=0; i<num_clients; i++)
  {
    TestPerformanceClient * performance_client = new TestPerformanceClient();
    performance_client->pipe_name = GetPipeNameFromTestName();
    performance_client->ready_flag = &ready_flag;
    performance_client->completed = false;
    performance_client->runtime_seconds = 0.0;
    performance_client->num_calls = num_calls;
    clients.push_back(performance_client);
  }

  //Start the threads
  printf("Creating %d threads.\n", num_clients);
  for(size_t i=0; i<num_clients; i++)
  {
    TestPerformanceClient * performance_client = clients[i];

    Status s = performance_client->thread_->Start();
    ASSERT_TRUE( s.Success() ) << s.GetDescription();

    HANDLE hThread = performance_client->thread_->GetHandle();
    DWORD dwThreadId = performance_client->thread_->GetId();
    printf("Thread index=%d, handle=0x%x, id=0x%x created.\n", i, hThread, dwThreadId);
  }

  // Initiate all threads to start calling the server
  printf("Initiating all threads make calls.\n");
  ready_flag = true;

  // Wait for all the threads to complete
  printf("Waiting for all threads to complete.\n");
  for(size_t i=0; i<num_clients; i++)
  {
    TestPerformanceClient * performance_client = clients[i];

    while(!performance_client->completed)
    {
      ra::timing::Millisleep(100);
    }
  }

  // Wait for the client threads to exit
  printf("Waiting for all threads to exit.\n");
  for(size_t i=0; i<num_clients; i++)
  {
    TestPerformanceClient * performance_client = clients[i];
    performance_client->thread_->Join();
  }

  // Ready to shutdown the server
  printf("Shutting down server.\n");
  s = object.server.Shutdown();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for the shutdown thread to complete
  thread.Join();

  // Print performance of each threads
  for(size_t i=0; i<num_clients; i++)
  {
    TestPerformanceClient * performance_client = clients[i];

    double calls_per_seconds = double(num_calls) / performance_client->runtime_seconds;
    double seconds_per_call = 1.0 / calls_per_seconds;
    double ms_per_call = seconds_per_call * 1000.0;
    double us_per_call = ms_per_call * 1000.0;

    printf("Thread %02d made %d calls in %f seconds. In average, that makes %.1f calls/second or %.1f microseconds for each call\n", i, num_calls, performance_client->runtime_seconds, calls_per_seconds, us_per_call);
  }

  // Destroy all clients
  for(size_t i=0; i<num_clients; i++)
  {
    TestPerformanceClient * performance_client = clients[i];
    delete performance_client;
  }
  clients.clear();
}
