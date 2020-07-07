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

#include "pbop/Mutex.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace pbop
{

  struct Mutex::PImpl
  {
    HANDLE mutex;
  };

  Mutex::Mutex() :
    impl_(new Mutex::PImpl())
  {
    impl_->mutex = CreateMutex(NULL, FALSE, NULL);
  }

  Mutex::~Mutex()
  {
    if (impl_)
    {
      CloseHandle(impl_->mutex);
      delete impl_;
    }
    impl_ = NULL;
  }

  void Mutex::Lock()
  {
    if (impl_)
    {
      WaitForSingleObject(impl_->mutex, INFINITE);
    }
  }

  void Mutex::Unlock()
  {
    if (impl_)
    {
      ReleaseMutex(impl_->mutex);
    }
  }

}; //namespace pbop
