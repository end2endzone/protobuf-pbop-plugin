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

#include "TestReadWriteLock.h"

#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

#include <Windows.h>

#include "pbop/Thread.h"
#include "pbop/ThreadBuilder.h"
#include "pbop/ReadWriteLock.h"

using namespace pbop;

void TestReadWriteLock::SetUp()
{
}

void TestReadWriteLock::TearDown()
{
}

extern std::string GetPipeNameFromTestName();
extern std::string GetThreadPrintPrefix();

struct StatsCounter
{
  ReadWriteLock lock;
  bool reader1; //true when a ThreadedReader with id 1 is locking
  bool reader2; //true when a ThreadedReader with id 2 is locking
  bool reader3; //true when a ThreadedReader with id 3 is locking
  bool reader4; //true when a ThreadedReader with id 4 is locking
  bool writer;  //true when a ThreadedWriter is locking
};

class ThreadedReader
{
public:
  StatsCounter * stats;
  Thread * thread;
  int reader_id;
  bool found_other_readers; //true when this instance has found another readers that was running at the same time as itself

  ThreadedReader()
  {
    stats = NULL;
    thread = new ThreadBuilder<ThreadedReader>(this, &ThreadedReader::Run);
    reader_id = 0;
    found_other_readers = false;
  }
  ~ThreadedReader()
  {
    thread->SetInterrupt();
    thread->Join();
    delete thread;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();

    double start_time_seconds = ra::timing::GetMillisecondsTimer();
    double end_time_seconds = ra::timing::GetMillisecondsTimer();
    double elapsed_time_seconds = end_time_seconds - start_time_seconds;

    // Run for 2 seconds
    while(elapsed_time_seconds <= 2.0 && !thread->IsInterrupted())
    {
      stats->lock.LockRead();

      // Set our "reading" flag
      switch(reader_id)
      {
      case 1:
        stats->reader1 = true;
        break;
      case 2:
        stats->reader2 = true;
        break;
      case 3:
        stats->reader3 = true;
        break;
      case 4:
        stats->reader4 = true;
        break;
      default:
        return 1;
      };

      // Loop for other readers
      for(int i=0; i<25; i++)
      {
        // Check other readers state
        for(int j=1; j<=4; j++)
        {
          if (j == reader_id)
            continue;
          switch(j)
          {
          case 1:
            if (stats->reader1) found_other_readers = true;
            break;
          case 2:
            if (stats->reader2) found_other_readers = true;
            break;
          case 3:
            if (stats->reader3) found_other_readers = true;
            break;
          case 4:
            if (stats->reader4) found_other_readers = true;
            break;
          default:
            return 2;
          };
        }

        // Wait a little before checking again
        Sleep(10);
      }

      // Unset our "reading" flag
      switch(reader_id)
      {
      case 1:
        stats->reader1 = false;
        break;
      case 2:
        stats->reader2 = false;
        break;
      case 3:
        stats->reader3 = false;
        break;
      case 4:
        stats->reader4 = false;
        break;
      default:
        return 1;
      };
      stats->lock.UnlockRead();

      // Compute elapsed time
      end_time_seconds = ra::timing::GetMillisecondsTimer();
      elapsed_time_seconds = end_time_seconds - start_time_seconds;
    }

    return 0;
  }
};

class ThreadedWriter
{
public:
  StatsCounter * stats;
  Thread * thread;
  bool found_other_readers; //true when this instance has found another readers that was running at the same time as itself

  ThreadedWriter()
  {
    stats = NULL;
    thread = new ThreadBuilder<ThreadedWriter>(this, &ThreadedWriter::Run);
    found_other_readers = false;
  }
  ~ThreadedWriter()
  {
    thread->SetInterrupt();
    thread->Join();
    delete thread;
  }

  DWORD Run()
  {
    std::string prefix = GetThreadPrintPrefix();

    double start_time_seconds = ra::timing::GetMillisecondsTimer();
    double end_time_seconds = ra::timing::GetMillisecondsTimer();
    double elapsed_time_seconds = end_time_seconds - start_time_seconds;

    // Run for 2 seconds
    while(elapsed_time_seconds <= 2.0 && !thread->IsInterrupted())
    {
      stats->lock.LockWrite();

      // Loop for other readers
      for(int i=0; i<5; i++)
      {
        // Check other readers state
        for(int j=1; j<=4; j++)
        {
          switch(j)
          {
          case 1:
            if (stats->reader1) found_other_readers = true;
            break;
          case 2:
            if (stats->reader2) found_other_readers = true;
            break;
          case 3:
            if (stats->reader3) found_other_readers = true;
            break;
          case 4:
            if (stats->reader4) found_other_readers = true;
            break;
          default:
            return 2;
          };
        }

        // Wait a little before checking again
        Sleep(10);
      }

      stats->lock.UnlockWrite();

      // Compute elapsed time
      end_time_seconds = ra::timing::GetMillisecondsTimer();
      elapsed_time_seconds = end_time_seconds - start_time_seconds;
    }

    return 0;
  }
};

TEST_F(TestReadWriteLock, testMultipleReadersAtSameTime)
{
  ThreadedReader r1;
  ThreadedReader r2;
  ThreadedReader r3;
  ThreadedReader r4;

  StatsCounter stats;
  stats.reader1 = false;
  stats.reader2 = false;
  stats.reader3 = false;
  stats.reader4 = false;
  stats.writer = false;

  // Init the readers
  r1.reader_id = 1;
  r2.reader_id = 2;
  r3.reader_id = 3;
  r4.reader_id = 4;

  r1.stats = &stats;
  r2.stats = &stats;
  r3.stats = &stats;
  r4.stats = &stats;

  // Start the reader threads
  Status s1 = r1.thread->Start();
  Status s2 = r2.thread->Start();
  Status s3 = r3.thread->Start();
  Status s4 = r4.thread->Start();
  ASSERT_TRUE( s1.Success() ) << s1.GetDescription();
  ASSERT_TRUE( s2.Success() ) << s2.GetDescription();
  ASSERT_TRUE( s3.Success() ) << s3.GetDescription();
  ASSERT_TRUE( s4.Success() ) << s4.GetDescription();

  // Wait for all readers to complete
  r1.thread->Join();
  r2.thread->Join();
  r3.thread->Join();
  r4.thread->Join();

  // Assert all readers should have seen another reader working at the sametime
  ASSERT_TRUE( r1.found_other_readers );
  ASSERT_TRUE( r2.found_other_readers );
  ASSERT_TRUE( r3.found_other_readers );
  ASSERT_TRUE( r4.found_other_readers );
}

TEST_F(TestReadWriteLock, testWriterBlocksAllReaders)
{
  ThreadedReader r1;
  ThreadedReader r2;
  ThreadedReader r3;
  ThreadedReader r4;
  ThreadedWriter w1;

  StatsCounter stats;
  stats.reader1 = false;
  stats.reader2 = false;
  stats.reader3 = false;
  stats.reader4 = false;
  stats.writer = false;

  // Init the readers
  r1.reader_id = 1;
  r2.reader_id = 2;
  r3.reader_id = 3;
  r4.reader_id = 4;
  
  r1.stats = &stats;
  r2.stats = &stats;
  r3.stats = &stats;
  r4.stats = &stats;
  w1.stats = &stats;

  // Start the reader & writer threads
  Status r1_status = r1.thread->Start();
  Status r2_status = r2.thread->Start();
  Status r3_status = r3.thread->Start();
  Status r4_status = r4.thread->Start();
  Status w1_status = w1.thread->Start();
  ASSERT_TRUE( r1_status.Success() ) << r1_status.GetDescription();
  ASSERT_TRUE( r2_status.Success() ) << r2_status.GetDescription();
  ASSERT_TRUE( r3_status.Success() ) << r3_status.GetDescription();
  ASSERT_TRUE( r4_status.Success() ) << r4_status.GetDescription();
  ASSERT_TRUE( w1_status.Success() ) << w1_status.GetDescription();

  // Wait for all readers and writer to complete
  r1.thread->Join();
  r2.thread->Join();
  r3.thread->Join();
  r4.thread->Join();
  w1.thread->Join();

  // Assert all readers should have seen another reader working at the sametime
  ASSERT_TRUE( r1.found_other_readers );
  ASSERT_TRUE( r2.found_other_readers );
  ASSERT_TRUE( r3.found_other_readers );
  ASSERT_TRUE( r4.found_other_readers );

  // Assert the writer must not have seen any readers working at the same time as himself
  ASSERT_FALSE( w1.found_other_readers );
}
