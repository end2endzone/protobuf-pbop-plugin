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

#include "pbop/ReadWriteLock.h"
#include "pbop/CriticalSection.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>

namespace pbop
{
  // Inspired from https://stackoverflow.com/questions/1008726/win32-read-write-lock-using-only-critical-sections

  struct ReadWriteLock::PImpl
  {
    CriticalSection counters_lock;
    CriticalSection writer_lock;
    HANDLE no_readers_event;
    int num_readers;
    bool waiting_writer;
  };

  ReadWriteLock::ReadWriteLock() :
    impl_(new ReadWriteLock::PImpl())
  {
    // Could use a semaphore as well.  There can only be one waiter ever,
    // so I'm showing an auto-reset event here.
    impl_->no_readers_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    impl_->num_readers = 0;
    impl_->waiting_writer = false;
  }

  ReadWriteLock::~ReadWriteLock()
  {
    if (impl_)
    {
      if (impl_->no_readers_event)
        CloseHandle(impl_->no_readers_event);
      impl_->no_readers_event = NULL;

      delete impl_;
    }
    impl_ = NULL;
  }

  void ReadWriteLock::LockRead()
  {
    if (impl_)
    {
      // We need to lock the writer_lock too, otherwise a writer could
      // do the whole of LockWrite() after the num_readers changed
      // from 0 to 1, but before the event was reset.
      impl_->writer_lock.Lock();
      impl_->counters_lock.Lock();
      impl_->num_readers++;
      impl_->counters_lock.Unlock();
      impl_->writer_lock.Unlock();
    }
  }

  void ReadWriteLock::UnlockRead()
  {
    if (impl_)
    {
      impl_->counters_lock.Lock();
      assert (impl_->num_readers > 0);
      if (--impl_->num_readers == 0)
      {
          if (impl_->waiting_writer)
          {
              // Clear waiting_writer here to avoid taking counters_lock
              // again in LockWrite().
              impl_->waiting_writer = false;
              SetEvent(impl_->no_readers_event);
          }
      }
      impl_->counters_lock.Unlock();
    }
  }

  void ReadWriteLock::LockWrite()
  {
    if (impl_)
    {
      impl_->writer_lock.Lock();
      // num_readers cannot become non-zero within the writer_lock CS,
      // but it can become zero...
      if (impl_->num_readers > 0) {
          impl_->counters_lock.Lock();

          // So test it again.
          if (impl_->num_readers > 0) {
              impl_->waiting_writer = true;
              impl_->counters_lock.Unlock();
              WaitForSingleObject(impl_->no_readers_event, INFINITE);
          } else {
              // How lucky, no need to wait.
              impl_->counters_lock.Unlock();
          }
      }

      // writer_lock remains locked.
    }
  }

  void ReadWriteLock::UnlockWrite()
  {
    if (impl_)
    {
      impl_->writer_lock.Unlock();
    }
  }

}; //namespace pbop
