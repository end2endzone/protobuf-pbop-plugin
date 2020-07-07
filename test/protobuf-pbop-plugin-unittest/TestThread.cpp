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

#include "TestThread.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"

#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

using namespace pbop;

static const HANDLE NULL_HANDLE = 0;

void TestThread::SetUp()
{
}

void TestThread::TearDown()
{
}

class SleepObject
{
public:
  SleepObject(DWORD sleep_time) : 
      sleep_time_(sleep_time),
      thread_(NULL)
  {
  }

  DWORD Run()
  {
    if (sleep_time_ == 0)
      return 0;

    double time_start_sec = ra::timing::GetMillisecondsTimer();
    double time_end_sec = time_start_sec + (sleep_time_/1000.0);

    while( ra::timing::GetMillisecondsTimer() < time_end_sec && !IsInterrupted() )
    {
      Sleep(10);
    }

    return 0;
  }

  void SetThread(Thread * thread)
  {
    thread_ = thread;
  }

  bool IsInterrupted()
  {
    if (thread_)
    {
      return thread_->IsInterrupted();
    }
    return false;
  }

private:
  DWORD sleep_time_;
  Thread * thread_;
};

TEST_F(TestThread, testBase)
{
  SleepObject object(2000);

  ThreadBuilder<SleepObject> thread(&object, &SleepObject::Run);

  double time_start_sec = ra::timing::GetMillisecondsTimer();

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  ASSERT_TRUE( thread.IsRunning() );

  // Wait for thread to terminate
  thread.Join();

  ASSERT_FALSE( thread.IsRunning() );

  // Assert running time should be near 2000 ms
  double time_end_sec = ra::timing::GetMillisecondsTimer();
  double elapsed_time_sec = time_end_sec - time_start_sec;
  double elapsed_time_ms = elapsed_time_sec * 1000.0;
  ASSERT_NEAR(2000, elapsed_time_ms, 200);
}

TEST_F(TestThread, testIsRunning)
{
  SleepObject object(0);

  ThreadBuilder<SleepObject> thread(&object, &SleepObject::Run);

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  Sleep(300);

  // At this point, the thread should be terminated without having to call Join()
  ASSERT_FALSE( thread.IsRunning() );
}

TEST_F(TestThread, testQuickTerminate)
{
  SleepObject object(0);

  ThreadBuilder<SleepObject> thread(&object, &SleepObject::Run);

  double time_start_sec = ra::timing::GetMillisecondsTimer();

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for thread to terminate
  thread.Join();

  // Assert running time should be quick
  double time_end_sec = ra::timing::GetMillisecondsTimer();
  double elapsed_time_sec = time_end_sec - time_start_sec;
  double elapsed_time_ms = elapsed_time_sec * 1000.0;
  ASSERT_NEAR(20, elapsed_time_ms, 20);
}

TEST_F(TestThread, testRunTwice)
{
  SleepObject object(1000);

  ThreadBuilder<SleepObject> thread(&object, &SleepObject::Run);

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for the thread to be running
  Sleep(300);
  ASSERT_TRUE( thread.IsRunning() );
  ASSERT_FALSE( thread.IsInterrupted() );

  // Expect valid handle and thread id
  HANDLE hThread1 = thread.GetHandle();
  DWORD dwThreadId1 = thread.GetId();
  ASSERT_NE( NULL_HANDLE, hThread1 );
  ASSERT_NE( INVALID_HANDLE_VALUE, hThread1 );
  ASSERT_NE( 0, dwThreadId1 );

  // Try to start the thread again (expect a failure)
  s = thread.Start();
  ASSERT_FALSE( s.Success() ) << s.GetDescription();

  // Wait for thread to terminate
  thread.Join();

  ASSERT_FALSE( thread.IsRunning() );
  ASSERT_FALSE( thread.IsInterrupted() );

  // Expect handle and id are still identical
  HANDLE hThread2 = thread.GetHandle();
  DWORD dwThreadId2 = thread.GetId();
  ASSERT_EQ( hThread2, hThread1 );
  ASSERT_EQ( dwThreadId2, dwThreadId1 );

  // Try to start the thread again (expect a success)
  s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for the thread to be running
  Sleep(300);
  ASSERT_TRUE( thread.IsRunning() );
  ASSERT_FALSE( thread.IsInterrupted() );

  // Expect new handle and thread id
  HANDLE hThread3 = thread.GetHandle();
  DWORD dwThreadId3 = thread.GetId();
  //ASSERT_NE( hThread3, hThread2 ); // Windows seems to be reusing the same handles if they are available
  ASSERT_NE( dwThreadId3, dwThreadId2 );

  // Wait for thread to terminate
  thread.Join();
}

TEST_F(TestThread, testInterruption)
{
  SleepObject object(2000);

  ThreadBuilder<SleepObject> thread(&object, &SleepObject::Run);

  object.SetThread(&thread);

  double time_start_sec = ra::timing::GetMillisecondsTimer();

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  ASSERT_TRUE( thread.IsRunning() );

  // Wait for the thread to be running
  Sleep(300);
  ASSERT_TRUE( thread.IsRunning() );
  ASSERT_FALSE( thread.IsInterrupted() );

  // Tell the thread to exit gracefully.
  thread.SetInterrupt();
  ASSERT_TRUE( thread.IsInterrupted() );

  // Wait for thread to terminate
  thread.Join();

  ASSERT_FALSE( thread.IsRunning() );
  ASSERT_TRUE( thread.IsInterrupted() );

  // Assert running time should be near 300 ms (not the expected 2000 ms)
  double time_end_sec = ra::timing::GetMillisecondsTimer();
  double elapsed_time_sec = time_end_sec - time_start_sec;
  double elapsed_time_ms = elapsed_time_sec * 1000.0;
  ASSERT_NEAR(300, elapsed_time_ms, 50);

  // Try to start the thread again (expect a success)
  s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Assert the interruption flag was reset
  ASSERT_FALSE( thread.IsInterrupted() );

  // Wait for thread to terminate
  thread.Join();
}

TEST_F(TestThread, testJoin)
{
  SleepObject object(200);

  ThreadBuilder<SleepObject> thread(&object, &SleepObject::Run);

  // Call join. Expect the call to be non-clocking since the thread is not running
  thread.Join();

  // Start the thread
  Status s = thread.Start();
  ASSERT_TRUE( s.Success() ) << s.GetDescription();

  // Wait for thread to terminate
  thread.Join();

  // Call join again. Expect the call to be non-clocking since the thread is terminated
  thread.Join();
}
