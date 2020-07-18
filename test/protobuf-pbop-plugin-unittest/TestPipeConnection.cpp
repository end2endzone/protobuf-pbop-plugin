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

std::string GetThreadPrintPrefix()
{
  DWORD dwThreadId = GetCurrentThreadId();

  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "Thread 0x%x", dwThreadId);

  return buffer;
}

// Fill a buffer with a Fibonacci sequence starting at the given seed value
void FibonacciFill(std::string & output, size_t target_size, char seed)
{
  output.clear();

  output.reserve(target_size);
  output.resize(target_size);
  
  if (output.size() != target_size)
    return; //error

  // We need to build the sequence as fast as possible.
  // Directly write in the buffer to skip std::string offset validation
  // which is painfully slow in debug mode.
  char * buffer = &output[0];
  buffer[0] = seed;
  buffer[1] = seed;

  char c1 = seed;
  char c2 = seed;
  for(size_t i=2; i<target_size; i++)
  {
    char sum = c1 + c2;
    buffer[i] = sum;

    // Shift right
    c1 = c2;
    c2 = sum;
  }
}

bool IsFibonacci(const std::string & buffer)
{
  if (buffer.size() < 2)
    return false;

  size_t size = buffer.size();

  char c1 = buffer[0];
  char c2 = buffer[1];

  for(size_t i=2; i<(size-1); i++)
  {
    char sum = c1 + c2;
    if (buffer[i] != sum)
      return false;

    c1 = c2;
    c2 = sum;
  }

  return true;
}

TEST_F(TestPipeConnection, testFibonacciValidators)
{
  std::string buffer;
  FibonacciFill(buffer, 15, 6);

  ASSERT_EQ(15, buffer.size());

  ASSERT_TRUE(IsFibonacci(buffer));
}

class TestBlankServer
{
public:
  Server server;
  std::string pipe_name;

  TestBlankServer() {}
  ~TestBlankServer() {}

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();

    printf("%s: Starting server in blocking mode.\n", prefix.c_str());
    Status status = server.Run(pipe_name.c_str());
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }
    printf("%s: Server returned.\n", prefix.c_str());

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
    std::string prefix = GetThreadPrintPrefix();

    printf("%s: Connecting to pipe: %s\n", prefix.c_str(), pipe_name.c_str());
    connection = new PipeConnection();
    Status status = connection->Connect(pipe_name.c_str());
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }
    printf("%s: Connected!\n", prefix.c_str());

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

class ThreadedDummyConnector
{
public:
  PipeConnection connection;
  std::string pipe_name;
  Thread * thread;

  ThreadedDummyConnector()
  {
    thread = new ThreadBuilder<ThreadedDummyConnector>(this, &ThreadedDummyConnector::Run);
  }
  ~ThreadedDummyConnector()
  {
    thread->SetInterrupt();
    thread->Join();
    delete thread;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();

    // Wait a little to let the server listens for connections.
    Sleep(500);

    printf("%s: Connecting to pipe: %s\n", prefix.c_str(), pipe_name.c_str());
    Status s = connection.Connect(pipe_name.c_str());
    if (!s.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, s.GetCode(), s.GetDescription().c_str());
      return s.GetCode();
    }
    printf("%s: Connected!\n", prefix.c_str());

    // Loop until the test is done to prevent destroying
    // the connection before the end of the test.
    while (!thread->IsInterrupted())
    {
      Sleep(100);
    }

    return 0;
  }
};

TEST_F(TestPipeConnection, testReadTimeout)
{
  ThreadedDummyConnector object;
  object.pipe_name = GetPipeNameFromTestName();

  // Start the client thread
  Status s = object.thread->Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Waiting for the incomming pipe connection
  PipeConnection * connection = NULL;
  s = PipeConnection::Listen(object.pipe_name.c_str(), &connection, NULL);
  ASSERT_TRUE( s.Success() ) << s.GetDescription();
  ASSERT_TRUE( connection != NULL );

  double start_time_seconds = ra::timing::GetMillisecondsTimer();

  std::string buffer;
  s = connection->Read(buffer, 1500);
  ASSERT_EQ( STATUS_CODE_TIMED_OUT, s.GetCode() );

  //compute elapsed time
  double end_time_seconds = ra::timing::GetMillisecondsTimer();
  double elapsed_time_seconds = end_time_seconds - start_time_seconds;
  ASSERT_NEAR(1.500, elapsed_time_seconds, 100); //allow 100ms difference 

  delete connection;
}

class ThreadedClientBlockWriter
{
public:
  PipeConnection connection;
  std::string pipe_name;
  Thread * thread;
  size_t block_size;
  Status status;

  ThreadedClientBlockWriter()
  {
    block_size = 1024;
    thread = new ThreadBuilder<ThreadedClientBlockWriter>(this, &ThreadedClientBlockWriter::Run);
  }
  ~ThreadedClientBlockWriter()
  {
    thread->SetInterrupt();
    thread->Join();
    delete thread;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();

    status = Status::OK;

    // Wait a little to let the server listens for connections.
    Sleep(500);

    printf("%s: Connecting to pipe: %s\n", prefix.c_str(), pipe_name.c_str());
    status = connection.Connect(pipe_name.c_str());
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }
    printf("%s: Connected!\n", prefix.c_str());

    // Loop until the test request to stop
    std::string buffer;
    char c = '\0'; // The character that we send. Different each loop
    while (!thread->IsInterrupted())
    {
      // Prepare the new buffered value
      c++;
      FibonacciFill(buffer, block_size, c);

      // Send to the connection
      status = connection.Write(buffer);
      if (!status.Success())
      {
        printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
        return status.GetCode();
      }
    }

    return 0;
  }
};

// This test writes messages that are bigger in length than the pipe's read and write buffer sizes.
TEST_F(TestPipeConnection, testBiggerMessageThanListenBuffer)
{
  ThreadedClientBlockWriter object;
  object.pipe_name = GetPipeNameFromTestName();
  object.block_size = 1024;

  // Start the client thread
  Status s = object.thread->Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Waiting for the incomming pipe connection
  PipeConnection * connection = NULL;
  PipeConnection::ListenOptions options = {0};
  options.buffer_size = 200;
  s = PipeConnection::Listen(object.pipe_name.c_str(), &connection, &options);
  ASSERT_TRUE( s.Success() ) << s.GetDescription();
  ASSERT_TRUE( connection != NULL );

  double start_time_seconds = ra::timing::GetMillisecondsTimer();
  double end_time_seconds = ra::timing::GetMillisecondsTimer();
  double elapsed_time_seconds = end_time_seconds - start_time_seconds;

  // Read data for 0.5 seconds
  while(elapsed_time_seconds <= 0.5)
  {
    // Read data
    std::string buffer;
    s = connection->Read(buffer, 5000);
    ASSERT_TRUE( s.Success() ) << s.GetDescription();

    // Assert that content is full and size of received buffer
    // is bigger than size of the pipe buffer. This assumes that
    // server have concatenated multiple smaller block.
    ASSERT_EQ( object.block_size, buffer.size() );

    // Assert content
    ASSERT_TRUE( IsFibonacci(buffer) );

    // Compute elapsed time
    end_time_seconds = ra::timing::GetMillisecondsTimer();
    elapsed_time_seconds = end_time_seconds - start_time_seconds;
  }

  // Tell the flooding thread to stop
  object.thread->SetInterrupt();
  // Read again to make sure that write buffer is not full (blocking)
  // to allow the thread to exit.
  std::string buffer;
  connection->Read(buffer, 5000);
  object.thread->Join();

  // Assert no error found in the thread
  ASSERT_TRUE( object.status.Success() ) << object.status.GetDescription();
  
  delete connection;
}

// This test writes messages that are bigger in length than the internal Read() buffer size.
TEST_F(TestPipeConnection, testBiggerMessageThanReadBuffer)
{
  ThreadedClientBlockWriter object;
  object.pipe_name = GetPipeNameFromTestName();
  object.block_size = 12000; // Internal Read() buffer size is 10240

  // Start the client thread
  Status s = object.thread->Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Waiting for the incomming pipe connection
  PipeConnection * connection = NULL;
  PipeConnection::ListenOptions options = {0};
  options.buffer_size = 1024;
  s = PipeConnection::Listen(object.pipe_name.c_str(), &connection, &options);
  ASSERT_TRUE( s.Success() ) << s.GetDescription();
  ASSERT_TRUE( connection != NULL );

  double start_time_seconds = ra::timing::GetMillisecondsTimer();
  double end_time_seconds = ra::timing::GetMillisecondsTimer();
  double elapsed_time_seconds = end_time_seconds - start_time_seconds;

  // Read data for 0.5 seconds
  while(elapsed_time_seconds <= 0.5)
  {
    // Read data
    std::string buffer;
    s = connection->Read(buffer, 5000);
    ASSERT_TRUE( s.Success() ) << s.GetDescription();

    // Assert that content is full and size of received buffer
    // is bigger than size of the pipe buffer. This assumes that
    // server have concatenated multiple smaller block.
    ASSERT_EQ( object.block_size, buffer.size() );

    // Assert content
    ASSERT_TRUE( IsFibonacci(buffer) );

    // Compute elapsed time
    end_time_seconds = ra::timing::GetMillisecondsTimer();
    elapsed_time_seconds = end_time_seconds - start_time_seconds;
  }

  // Tell the flooding thread to stop
  object.thread->SetInterrupt();
  // Read again to make sure that write buffer is not full (blocking)
  std::string buffer;
  connection->Read(buffer, 5000);
  object.thread->Join();

  // Assert no error found in the thread
  ASSERT_TRUE( object.status.Success() ) << object.status.GetDescription();
  
  delete connection;
}

class ThreadedPlainListener
{
public:
  PipeConnection * connection;
  std::string pipe_name;
  Thread * thread;
  Status status;

  ThreadedPlainListener()
  {
    connection = NULL;
    thread = new ThreadBuilder<ThreadedPlainListener>(this, &ThreadedPlainListener::Run);
  }
  ~ThreadedPlainListener()
  {
    thread->SetInterrupt();
    thread->Join();
    delete thread;

    if (connection)
      delete connection;
    connection = NULL;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();

    status = Status::OK;

    printf("%s: Listening for incomming connection to pipe: %s\n", prefix.c_str(), pipe_name.c_str());
    status = PipeConnection::Listen(pipe_name.c_str(), &connection, NULL);
    if (!status.Success())
    {
      printf("Error in %s(): %d, %s\n", __FUNCTION__, status.GetCode(), status.GetDescription().c_str());
      return status.GetCode();
    }
    printf("%s: Received connection.\n", prefix.c_str());

    // Wait for the user to ask for exit
    while (!thread->IsInterrupted())
    {
      Sleep(100);
    }

    return 0;
  }
};

TEST_F(TestPipeConnection, DISABLED_testFillWriteBuffer)
{
  // From https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
  //  If the pipe buffer is full when an application uses the WriteFile function to write to a pipe,
  //  the write operation may not finish immediately. The write operation will be completed when a
  //  read operation (using the ReadFile function) makes more system buffer space available for the pipe.


  ThreadedPlainListener object;
  object.pipe_name = GetPipeNameFromTestName();

  // Start the client thread
  Status s = object.thread->Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Waiting for the listener to start listening...
  Sleep(100);

  // Connect to the listener
  PipeConnection connection;
  s = connection.Connect(object.pipe_name.c_str());
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Write a first message. This message should be successful.
  std::string buffer;
  FibonacciFill(buffer, 1024, 'a');
  s = connection.Write(buffer);
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  double start_time_seconds = ra::timing::GetMillisecondsTimer();
  double end_time_seconds = ra::timing::GetMillisecondsTimer();
  double elapsed_time_seconds = end_time_seconds - start_time_seconds;

  // Write data for 1.5 seconds
  while(elapsed_time_seconds <= 1.5 && s.Success())
  {
    // Change the content of the buffer
    char c = buffer[0];
    c++;
    FibonacciFill(buffer, 1024, c);

    s = connection.Write(buffer);

    // Compute elapsed time
    end_time_seconds = ra::timing::GetMillisecondsTimer();
    elapsed_time_seconds = end_time_seconds - start_time_seconds;
  }

  // The buffer should be full by now...
  ASSERT_EQ( STATUS_CODE_PIPE_ERROR, s.GetCode() );

  // Tell the listening thread to stop
  object.thread->SetInterrupt();
  object.thread->Join();

  // Assert no error found in the thread
  ASSERT_TRUE( object.status.Success() ) << object.status.GetDescription();
}
