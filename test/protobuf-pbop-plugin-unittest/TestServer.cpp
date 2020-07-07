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

#include "TestServer.h"

#include "pbop/Server.h"
#include "pbop/PipeConnection.h"

#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"

using namespace pbop;

std::string GetPipeNameFromTestName()
{
  const std::string current_test_name = ra::testing::GetTestQualifiedName();
#ifdef _WIN32
  std::string pipe_name = "\\\\.\\pipe\\" + current_test_name;
#else
#error Please define function GetPipenameFromUnitTest() for other platform
#endif  
  return pipe_name;
}

void TestServer::SetUp()
{
}

void TestServer::TearDown()
{
}

class TestShutdown
{
public:
  Server server;

  TestShutdown() {}
  ~TestShutdown() {}

  DWORD Run()
  {
    // Allow time for the server to start listening for connections
    while(!server.IsRunning())
    {
      ra::timing::Millisleep(500);
    }
    ra::timing::Millisleep(500);

    // Initiate shutdown process
    server.Shutdown();

    return 0;
  }
};

TEST_F(TestServer, testShutdown)
{
  std::string pipe_name = GetPipeNameFromTestName();

  TestShutdown object;

  ThreadBuilder<TestShutdown> thread(&object, &TestShutdown::Run);

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  printf("Starting server in blocking mode.\n");
  printf("Waiting for server function to return...\n");
  Status status = object.server.Run(pipe_name.c_str());
  printf("Server returned. Test is successful\n");

  // Wait for the shutdown thread to complete
  thread.Join();

  int a = 0;
}

class MyEventLoggingServer : public Server
{
public:
  MyEventLoggingServer() : 
    num_EventStartup           (0),
    num_EventShutdown          (0),
    num_EventListening         (0),
    num_EventConnection        (0),
    num_EventClientCreate      (0),
    num_EventClientDisconnected(0),
    num_EventClientDestroy     (0),
    num_EventClientError       (0)
    {
    }

  virtual ~MyEventLoggingServer() {}

  virtual void OnEvent(EventStartup            * e) { num_EventStartup           ++; LogEvent("EventStartup           "); };
  virtual void OnEvent(EventShutdown           * e) { num_EventShutdown          ++; LogEvent("EventShutdown          "); };
  virtual void OnEvent(EventListening          * e) { num_EventListening         ++; LogEvent("EventListening         "); };
  virtual void OnEvent(EventConnection         * e) { num_EventConnection        ++; LogEvent("EventConnection        "); };
  virtual void OnEvent(EventClientCreate       * e) { num_EventClientCreate      ++; LogEvent("EventClientCreate      "); };
  virtual void OnEvent(EventClientDisconnected * e) { num_EventClientDisconnected++; LogEvent("EventClientDisconnected"); };
  virtual void OnEvent(EventClientDestroy      * e) { num_EventClientDestroy     ++; LogEvent("EventClientDestroy     "); };
  virtual void OnEvent(EventClientError        * e) { num_EventClientError       ++; LogEvent("EventClientError       "); };

  int num_EventStartup           ;
  int num_EventShutdown          ;
  int num_EventListening         ;
  int num_EventConnection        ;
  int num_EventClientCreate      ;
  int num_EventClientDisconnected;
  int num_EventClientDestroy     ;
  int num_EventClientError       ;

  struct CallLog
  {
    std::string name;
    clock_t time;
  };
  std::vector<CallLog> call_logs_;

  void LogEvent(const char * name)
  {
    clock_t t = clock();

    CallLog c;
    c.name = name;
    c.time = t;

    call_logs_.push_back(c);

    Sleep(15);  //force the next event to be at a different time
  }

  void PrintCounts()
  {
    printf("Call counts:\n");
    printf("  EventStartup            = %d\n", num_EventStartup           );
    printf("  EventShutdown           = %d\n", num_EventShutdown          );
    printf("  EventListening          = %d\n", num_EventListening         );
    printf("  EventConnection         = %d\n", num_EventConnection        );
    printf("  EventClientCreate       = %d\n", num_EventClientCreate      );
    printf("  EventClientDisconnected = %d\n", num_EventClientDisconnected);
    printf("  EventClientDestroy      = %d\n", num_EventClientDestroy     );
    printf("  EventClientError        = %d\n", num_EventClientError       );
  }

  void PrintCallLogs()
  {
    printf("Call times:\n");
    for(size_t i=0; i<call_logs_.size(); i++)
    {
      const CallLog & c = call_logs_[i];
      printf("  %20s, %u\n", c.name.c_str(), c.time);
    }
  }
};

class TestEventsBasic
{
public:
  MyEventLoggingServer server;
  std::string pipe_name;

  TestEventsBasic() {}
  ~TestEventsBasic() {}

  DWORD Run()
  {
    printf("Starting server in blocking mode.\n");
    Status status = server.Run(pipe_name.c_str());
    printf("Server returned.\n");

    return 0;
  }
};

TEST_F(TestServer, testEventsBasic)
{
  TestEventsBasic object;

  object.pipe_name = GetPipeNameFromTestName();

  ThreadBuilder<TestEventsBasic> thread(&object, &TestEventsBasic::Run);

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Allow time for the server to start listening for connections
  while(!object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  // Initiate shutdown process
  object.server.Shutdown();

  // Wait for the shutdown thread to complete
  thread.Join();

  object.server.PrintCounts();
  object.server.PrintCallLogs();

  ASSERT_EQ(1, object.server.num_EventStartup           ); 
  ASSERT_EQ(1, object.server.num_EventShutdown          ); 
  ASSERT_EQ(1, object.server.num_EventListening         ); 
  ASSERT_EQ(0, object.server.num_EventConnection        ); 
  ASSERT_EQ(0, object.server.num_EventClientCreate      ); 
  ASSERT_EQ(0, object.server.num_EventClientDisconnected); 
  ASSERT_EQ(0, object.server.num_EventClientDestroy     ); 
  ASSERT_EQ(0, object.server.num_EventClientError       ); 

  int a = 0;
}

class TestEventsConnection
{
public:
  MyEventLoggingServer server;
  std::string pipe_name;

  TestEventsConnection() {}
  ~TestEventsConnection() {}

  DWORD Run()
  {
    printf("Starting server in blocking mode.\n");
    Status status = server.Run(pipe_name.c_str());
    printf("Server returned.\n");

    return 0;
  }
};

TEST_F(TestServer, testEventsConnection)
{
  TestEventsConnection object;

  object.pipe_name = GetPipeNameFromTestName();

  ThreadBuilder<TestEventsConnection> thread(&object, &TestEventsConnection::Run);

  int expected_EventStartup            = 0;
  int expected_EventShutdown           = 0;
  int expected_EventListening          = 0;
  int expected_EventConnection         = 0;
  int expected_EventClientCreate       = 0;
  int expected_EventClientDisconnected = 0;
  int expected_EventClientDestroy      = 0;
  int expected_EventClientError        = 0;
  ASSERT_EQ(expected_EventStartup           , object.server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , object.server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , object.server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , object.server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , object.server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, object.server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , object.server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , object.server.num_EventClientError       );

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Allow time for the server to start listening for connections
  while(!object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  // At this point, the server should be started and listening
  expected_EventStartup++;
  expected_EventListening++;
  ASSERT_EQ(expected_EventStartup           , object.server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , object.server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , object.server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , object.server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , object.server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, object.server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , object.server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , object.server.num_EventClientError       );

  // Run 2 connection loops
  for(size_t i=0; i<2; i++)
  {
    PipeConnection * pipe = new PipeConnection;
    Status s = pipe->Connect(object.pipe_name.c_str());
    ASSERT_TRUE( s.Success() ) << s.GetDescription();

    //Wait for the server to process this connection
    ra::timing::Millisleep(500);

    // At this point, the connection should be completed
    // The server should be back to listening again
    expected_EventConnection++;
    expected_EventClientCreate++;
    expected_EventListening++;
    ASSERT_EQ(expected_EventStartup           , object.server.num_EventStartup           );
    ASSERT_EQ(expected_EventShutdown          , object.server.num_EventShutdown          );
    ASSERT_EQ(expected_EventListening         , object.server.num_EventListening         );
    ASSERT_EQ(expected_EventConnection        , object.server.num_EventConnection        );
    ASSERT_EQ(expected_EventClientCreate      , object.server.num_EventClientCreate      );
    ASSERT_EQ(expected_EventClientDisconnected, object.server.num_EventClientDisconnected);
    ASSERT_EQ(expected_EventClientDestroy     , object.server.num_EventClientDestroy     );
    ASSERT_EQ(expected_EventClientError       , object.server.num_EventClientError       );

    // Disconnect from the server
    delete pipe;
    pipe = NULL;

    // Wait for the server to process this disconnection
    ra::timing::Millisleep(500);

    // At this point, the server should have noticed the closed connection
    expected_EventClientDisconnected++;
    expected_EventClientDestroy++;
    ASSERT_EQ(expected_EventStartup           , object.server.num_EventStartup           );
    ASSERT_EQ(expected_EventShutdown          , object.server.num_EventShutdown          );
    ASSERT_EQ(expected_EventListening         , object.server.num_EventListening         );
    ASSERT_EQ(expected_EventConnection        , object.server.num_EventConnection        );
    ASSERT_EQ(expected_EventClientCreate      , object.server.num_EventClientCreate      );
    ASSERT_EQ(expected_EventClientDisconnected, object.server.num_EventClientDisconnected);
    ASSERT_EQ(expected_EventClientDestroy     , object.server.num_EventClientDestroy     );
    ASSERT_EQ(expected_EventClientError       , object.server.num_EventClientError       );
  }

  // Initiate shutdown process
  object.server.Shutdown();

  // Allow time for the server to shutdown
  while(object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  // The server has shutdown
  expected_EventShutdown++;
  ASSERT_EQ(expected_EventStartup           , object.server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , object.server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , object.server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , object.server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , object.server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, object.server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , object.server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , object.server.num_EventClientError       );

  // Wait for the shutdown thread to complete
  thread.Join();

  object.server.PrintCounts();
  object.server.PrintCallLogs();

  int a = 0;
}
