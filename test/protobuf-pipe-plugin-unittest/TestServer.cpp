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
#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

#include <Windows.h>

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

std::string GetErrorDesription(DWORD code)
{
  const DWORD error_buffer_size = 10240;
  char error_buffer[error_buffer_size] = { 0 };
  ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    error_buffer,
    error_buffer_size - 1,
    NULL);
  std::string error_desc = error_buffer;
  return error_desc;
}

bool IsThreadAlive(HANDLE hThread)
{
  // https://stackoverflow.com/questions/301054/how-can-i-determine-if-a-win32-thread-has-terminated
  DWORD result = WaitForSingleObject(hThread, 0);
  DWORD dwLastError = 0;
  std::string desc;
  switch(result)
  {
  case WAIT_OBJECT_0:
    // the thread handle is signaled - the thread has terminated
    return false;
  case WAIT_TIMEOUT:
    // the thread handle is not signaled - the thread is still alive
    return true;
  case WAIT_FAILED:
    dwLastError = GetLastError();
    if (dwLastError == ERROR_INVALID_HANDLE)
    {
      // the handle is no longer associated with a thread - the thread has terminated
      return false;
    }
    else
      return true;
  default:
    return false;
  };
}

void WaitThreadExit(HANDLE hThread)
{
  bool alive = IsThreadAlive(hThread);
  while(alive)
  {
    // Wait a little more
    Sleep(100);

    // And check again
    alive = IsThreadAlive(hThread);
  }
}

DWORD WINAPI TestShutdownThread(LPVOID lpvParam)
{
  // Read the thread's parameters
  Server * server = static_cast<Server*>(lpvParam);

  // Allow time for the server to start listening for connections
  while(!server->IsRunning())
  {
    ra::timing::Millisleep(500);
  }
  ra::timing::Millisleep(500);

  // Initiate shutdown process
  server->Shutdown();

  return 1;
}

TEST_F(TestServer, testShutdown)
{
  std::string pipe_name = GetPipeNameFromTestName();

  Server server;

  // Create a thread that will shutdown this server.
  HANDLE hThread = NULL;
  DWORD  dwThreadId = 0;
  hThread = CreateThread(
    NULL,               // no security attribute
    0,                  // default stack size
    TestShutdownThread, // thread proc
    &server,            // thread parameter
    0,                  // not suspended
    &dwThreadId);       // returns thread ID
  ASSERT_FALSE(hThread == NULL);
  CloseHandle(hThread);

  printf("Starting server in blocking mode.\n");
  printf("Waiting for server function to return...\n");
  Status status = server.Run(pipe_name.c_str());
  printf("Server returned. Test is successful\n");

  // Wait for the shutdown thread to complete
  WaitThreadExit(hThread);

  int a = 0;
}

struct ServerThreadProcParams
{
  Server * server;
  std::string pipe_name;
};

DWORD WINAPI ServerThreadProc(LPVOID lpvParam)
{
  // Read the thread's parameters
  ServerThreadProcParams * params = static_cast<ServerThreadProcParams*>(lpvParam);
  Server * server = params->server;
  std::string pipe_name = params->pipe_name;

  printf("Starting server in blocking mode.\n");
  Status status = server->Run(pipe_name.c_str());
  printf("Server returned.\n");

  return 1;
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

TEST_F(TestServer, testEventsBasic)
{
  MyEventLoggingServer server;

  ServerThreadProcParams params;
  params.server = &server;
  params.pipe_name = GetPipeNameFromTestName();

  // Create a thread that will shutdown this server.
  HANDLE hThread = NULL;
  DWORD  dwThreadId = 0;
  hThread = CreateThread(
    NULL,               // no security attribute
    0,                  // default stack size
    ServerThreadProc,   // thread proc
    &params,            // thread parameter
    0,                  // not suspended
    &dwThreadId);       // returns thread ID
  ASSERT_FALSE(hThread == NULL);
  CloseHandle(hThread);

  // Allow time for the server to start listening for connections
  while(!server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  // Initiate shutdown process
  server.Shutdown();

  ASSERT_EQ(1, server.num_EventStartup           ); 
  ASSERT_EQ(1, server.num_EventShutdown          ); 
  ASSERT_EQ(1, server.num_EventListening         ); 
  ASSERT_EQ(0, server.num_EventConnection        ); 
  ASSERT_EQ(0, server.num_EventClientCreate      ); 
  ASSERT_EQ(0, server.num_EventClientDisconnected); 
  ASSERT_EQ(0, server.num_EventClientDestroy     ); 
  ASSERT_EQ(0, server.num_EventClientError       ); 

  // Wait for the shutdown thread to complete
  WaitThreadExit(hThread);

  server.PrintCounts();
  server.PrintCallLogs();

  int a = 0;
}

TEST_F(TestServer, testEventsConnection)
{
  MyEventLoggingServer server;

  ServerThreadProcParams params;
  params.server = &server;
  params.pipe_name = GetPipeNameFromTestName();

  int expected_EventStartup            = 0;
  int expected_EventShutdown           = 0;
  int expected_EventListening          = 0;
  int expected_EventConnection         = 0;
  int expected_EventClientCreate       = 0;
  int expected_EventClientDisconnected = 0;
  int expected_EventClientDestroy      = 0;
  int expected_EventClientError        = 0;
  ASSERT_EQ(expected_EventStartup           , server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , server.num_EventClientError       );

  // Create a thread that will shutdown this server.
  HANDLE hThread = NULL;
  DWORD  dwThreadId = 0;
  hThread = CreateThread(
    NULL,               // no security attribute
    0,                  // default stack size
    ServerThreadProc,   // thread proc
    &params,            // thread parameter
    0,                  // not suspended
    &dwThreadId);       // returns thread ID
  ASSERT_FALSE(hThread == NULL);
  CloseHandle(hThread);

  // Allow time for the server to start listening for connections
  while(!server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  // At this point, the server should be started and listening
  expected_EventStartup++;
  expected_EventListening++;
  ASSERT_EQ(expected_EventStartup           , server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , server.num_EventClientError       );

  PipeConnection * pipe1 = new PipeConnection;
  pipe1->Connect(params.pipe_name.c_str());

  //Wait for the server to process this connection
  ra::timing::Millisleep(500);

  // At this point, the connection should be completed
  // The server should be back to listening again
  expected_EventConnection++;
  expected_EventClientCreate++;
  expected_EventListening++;
  ASSERT_EQ(expected_EventStartup           , server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , server.num_EventClientError       );

  //disconnect from the server
  delete pipe1;
  pipe1 = NULL;

  //Wait for the server to process this disconnection
  ra::timing::Millisleep(500);

  // At this point, the server should have noticed the closed connection
  expected_EventClientDisconnected++;
  expected_EventClientDestroy++;
  ASSERT_EQ(expected_EventStartup           , server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , server.num_EventClientError       );

  // Initiate shutdown process
  server.Shutdown();

  // Allow time for the server to shutdown
  while(server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);

  // The server has shutdown
  expected_EventShutdown++;
  ASSERT_EQ(expected_EventStartup           , server.num_EventStartup           );
  ASSERT_EQ(expected_EventShutdown          , server.num_EventShutdown          );
  ASSERT_EQ(expected_EventListening         , server.num_EventListening         );
  ASSERT_EQ(expected_EventConnection        , server.num_EventConnection        );
  ASSERT_EQ(expected_EventClientCreate      , server.num_EventClientCreate      );
  ASSERT_EQ(expected_EventClientDisconnected, server.num_EventClientDisconnected);
  ASSERT_EQ(expected_EventClientDestroy     , server.num_EventClientDestroy     );
  ASSERT_EQ(expected_EventClientError       , server.num_EventClientError       );

  // Wait for the shutdown thread to complete
  WaitThreadExit(hThread);

  server.PrintCounts();
  server.PrintCallLogs();

  int a = 0;
}
