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

#ifndef LIB_PBOP_SCOPE_LOCK
#define LIB_PBOP_SCOPE_LOCK

#include "pbop/Mutex.h"
#include "pbop/CriticalSection.h"

namespace pbop
{

  class ScopeLock
  {
  private:
    Mutex * mutex_;
    CriticalSection * cs_;

  private:
    ScopeLock(const ScopeLock & copy); //disable copy constructor.
    ScopeLock & operator =(const ScopeLock & other); //disable assignment operator.

  public:
    ScopeLock(Mutex * mutex);
    ScopeLock(CriticalSection * cs);

    ~ScopeLock();
  };

}; //namespace pbop

#endif //LIB_PBOP_SCOPE_LOCK