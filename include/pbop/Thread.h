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

#ifndef LIB_PBOP_THREAD
#define LIB_PBOP_THREAD

//Define `HANDLE` ourselve to prevent a dependency on <Windows.h>.
//Using `#include <WinNT.h>` to only get the definition of `HANDLE` results in multiple errors:
//  WinNT.h(351): error C2146: syntax error : missing ';' before identifier 'WCHAR'
//  WinNT.h(351): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int
//  WinNT.h(355): error C2143: syntax error : missing ';' before '*'
//  WinNT.h(355): error C2040: 'PWSTR' : 'CONST' differs in levels of indirection from 'WCHAR *'
//  WinNT.h(355): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int
//  WinNT.h(357): error C2143: syntax error : missing ';' before '*'
//  WinNT.h(357): error C2371: 'WCHAR' : redefinition; different basic types
//Maybe because its missing include/header guards.
typedef void *PVOID;
typedef PVOID HANDLE;

#include "pbop/Status.h"

namespace pbop
{

  class Thread
  {
  public:
    Thread() {}
    virtual ~Thread() {}

  private:
    ///<summary>Disable copy constructor. Prevent copying of thread objects.</summary>
    Thread(const Thread & other) {}

    ///<summary>Disable assignment operator. Prevent assignment of thread objects.</summary>
    Thread & operator =(const Thread & other) {}

  public:

    /// <summary>
    /// Starts the threads by executing the method pointer of the object in a concurrent thread.
    /// </summary>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    virtual Status Start() = 0;

    /// <summary>
    /// Blocks the calling thread until this thread has stopped.
    /// </summary>
    virtual void Join() = 0;

    /// <summary>
    /// Set an interrupt request flag to tell the thread to exit nicely.
    /// </summary>
    virtual void SetInterrupt() = 0;

    /// <summary>
    /// Returns true if an interrupt request was set. Returns false otherwise.
    /// </summary>
    /// <returns>Returns true if an interrupt request was set. Returns false otherwise.</returns>
    virtual bool IsInterrupted() const = 0;

    /// <summary>
    /// Returns true if the thread is running.
    /// </summary>
    /// <returns>Returns true if the thread is running. Returns false otherwise.</returns>
    virtual bool IsRunning() const = 0;

    /// <summary>
    /// Get the thread handle.
    /// </summary>
    /// <returns>Returns the thread handle.</returns>
    virtual HANDLE GetHandle() const = 0;
    
    /// <summary>
    /// Get the thread id.
    /// </summary>
    /// <returns>Returns the thread id.</returns>
    virtual unsigned long GetId() const = 0;

  };

}; //namespace pbop

#endif //LIB_PBOP_THREAD