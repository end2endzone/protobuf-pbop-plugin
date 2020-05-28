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
