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

#include "TestPipeConnection.h"
#include "pbop/PipeConnection.h"
#include "pbop/Server.h"

#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"

using namespace pbop;

void TestPipeConnection::SetUp()
{
}

void TestPipeConnection::TearDown()
{
}

extern std::string GetPipeNameFromTestName();

class TestBlankServer
{
public:
  Server server;
  std::string pipe_name;

  TestBlankServer() {}
  ~TestBlankServer() {}

  DWORD Run()
  {
    printf("Starting server in blocking mode.\n");
    Status status = server.Run(pipe_name.c_str());
    printf("Server returned.\n");

    return 0;
  }
};

class TestClientForceClose
{
public:
  std::string pipe_name;
  PipeConnection * connection;

  TestClientForceClose() {}
  ~TestClientForceClose() {}

  DWORD Run()
  {
    connection = new PipeConnection();
    Status status = connection->Connect(pipe_name.c_str());
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }

    std::string buffer;
    status = connection->Read(buffer);
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }

    return 0;
  }
};

TEST_F(TestPipeConnection, DISABLED_testForceClose)
{
  TestBlankServer server_object;
  server_object.pipe_name = GetPipeNameFromTestName();

  ThreadBuilder<TestBlankServer> server_thread(&server_object, &TestBlankServer::Run);

  // Start the server thread
  Status s = server_thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Allow time for the server to start listening for connections
  while(!server_object.server.IsRunning())
  {
    ra::timing::Millisleep(100);
  }
  ra::timing::Millisleep(100);


  TestClientForceClose client_object;
  client_object.pipe_name = GetPipeNameFromTestName();

  ThreadBuilder<TestClientForceClose> client_thread(&client_object, &TestClientForceClose::Run);

  // Start the client thread
  s = client_thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  Sleep(1000);

  // Try to force close the client's connection
  client_object.connection->ForceClose();

  // Ready to shutdown the server
  printf("Shutting down server.\n");
  s = server_object.server.Shutdown();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for the shutdown thread to complete
  server_thread.Join();
}
