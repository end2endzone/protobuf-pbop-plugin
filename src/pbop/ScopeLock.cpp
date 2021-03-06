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

#include "pbop/ScopeLock.h"
#include <stdio.h>

namespace pbop
{

  ScopeLock::ScopeLock(Mutex * mutex) :
    mutex_(mutex),
    cs_(NULL),
    rw_(NULL)
  {
    mutex_->Lock();
  }

  ScopeLock::ScopeLock(CriticalSection * cs) :
    mutex_(NULL),
    cs_(cs),
    rw_(NULL)
  {
    cs->Lock();
  }

  ScopeLock::ScopeLock(ReadWriteLock * rw, Mode mode) :
    mutex_(NULL),
    cs_(NULL),
    rw_(rw),
    mode_(mode)
  {
    switch(mode_)
    {
    case READING:
      rw_->LockRead();
      break;
    case WRITING:
    default:
      rw_->LockWrite();
      break;
    };
  }

  ScopeLock::~ScopeLock()
  {
    if (mutex_)
    {
      mutex_->Unlock();
    }
    if (cs_)
    {
      cs_->Unlock();
    }
    if (rw_)
    {
      switch(mode_)
      {
      case READING:
        rw_->UnlockRead();
        break;
      case WRITING:
      default:
        rw_->UnlockWrite();
        break;
      };
    }

    mutex_ = NULL;
    cs_ = NULL;
  }

}; //namespace pbop
