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

#ifndef LIB_PBOP_THREADBUILDER
#define LIB_PBOP_THREADBUILDER

#include "pbop/Thread.h"
#include <string>

namespace pbop
{

  /// <summary>
  /// Template class for creating threads.
  /// Inpired from https://stackoverflow.com/questions/1372967/how-do-you-use-createthread-for-functions-which-are-class-members
  /// </summary>
  template<class T>
  class ThreadBuilder : public virtual Thread
  {
  private:
    ///<summary>Type definition to a pointer of a method of class T with no parameters</summary>
    typedef DWORD (T::*TMethodPointer)(void);

  protected:
    ///<summary>Handle of the thread</summary>
    HANDLE hThread_;
  private:
    ///<summary>Thread id of the thread. The value 0 means invalid.</summary>
    DWORD dwThreadID_;

    ///<summary>Mutex to signal an interrupt via ReleaseSemaphore().</summary>
    HANDLE hInterrupt_;

    ///<summary>Force that only one thread allowed to call start() mutex.</summary>
    HANDLE hSingleStart_;

    ///<summary>The object which owns the method that executes concurently.</summary>
    T* object_;

    ///<summary>The method pointer of the object of type T which is executed concurently.</summary>
    TMethodPointer lpMethod_;

  private:
    /// <summary>
    /// Target function argument of the CreateThread() function.
    /// </summary>
    /// <param name="thread_obj">A pointer to this Thread object.</param>
    /// <returns>Returns the thread exit code. Returns 0 on </returns>
    static DWORD Run(LPVOID thread_obj)
    {
      ThreadBuilder<T>* thread = (ThreadBuilder<T>*)thread_obj;
      return (thread->object_->*thread->lpMethod_) ();
    }

    ///<summary>Disable copy constructor. Prevent copying of thread objects.</summary>
    ThreadBuilder(const ThreadBuilder<T>& other) {}

    ///<summary>Disable assignment operator. Prevent assignment of thread objects.</summary>
    ThreadBuilder<T>& operator =(const ThreadBuilder<T>& other) {}

  public:
    /// <summary>
    /// Creates a new Thread object.
    /// </summary>
    /// <param name="object">The object which owns the method that executes concurently.</param>
    /// <param name="lpMethod">The method pointer of the object of type T which is executed concurently.</param>
    explicit ThreadBuilder(T* object, TMethodPointer lpMethod)
    {
      this->hThread_       = NULL;
      this->object_        = object;
      this->lpMethod_      = lpMethod;
      this->dwThreadID_    = 0;
      this->hInterrupt_    = CreateSemaphore(NULL, 1, 1, NULL);
      this->hSingleStart_  = CreateMutex(NULL, FALSE, NULL);
    }

    virtual ~ThreadBuilder(void)
    {
      Join();

      if (hInterrupt_)
        CloseHandle(hInterrupt_);
      if (hSingleStart_)
        CloseHandle(hSingleStart_);
      if (hThread_)
        CloseHandle(hThread_);
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

    class ScopeReleaseMutex
    {
    public:
      HANDLE hMutex_;

    public:
      ScopeReleaseMutex(HANDLE hMutex) : hMutex_(hMutex) {}
      ~ScopeReleaseMutex()
      {
        ReleaseMutex(hMutex_);
      }
    };

    Status Start()
    {
      ScopeReleaseMutex scope_release(hSingleStart_);

      if (WaitForSingleObject(hSingleStart_, 0) != WAIT_OBJECT_0)
      {
        return Status(STATUS_CODE_CANCELLED, "Another thread is already starting this thread.");
      }
      if (hThread_) // The thread had been started sometime in the past
      {
        // Is the thread still running?
        if (WaitForSingleObject(hThread_, 0) == WAIT_TIMEOUT)
        {
          return Status(STATUS_CODE_CANCELLED, "The thread is already running.");
        }

        // Close the handle of the previous running thread.
        CloseHandle(hThread_);
        hThread_ = NULL;
        dwThreadID_ = 0;
      }

      // Set or reset the 'not interrupted' semaphore state
      WaitForSingleObject(hInterrupt_, 0);

      hThread_ = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE) ThreadBuilder<T>::Run,
        this,
        0,
        &this->dwThreadID_);

      if (hThread_)
      {
        return Status::OK;
      }

      // CreateThread has failed.
      std::string error_description = std::string("CreateThread failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_CANCELLED, error_description);
    }

    inline void Join()
    {
      WaitForSingleObject(hThread_, INFINITE);
    }

    inline Status SetInterrupt()
    {
      if (hInterrupt_ == NULL)
        return Status(STATUS_CODE_CANCELLED, "SetInterrupt failed. Interrupt lock invalid");

      if (ReleaseSemaphore(hInterrupt_, 1, NULL) == FALSE)
      {
        std::string error_description = std::string("SetInterrupt failed: ") + GetErrorDesription(GetLastError());
        return Status(STATUS_CODE_CANCELLED, error_description);
      }
      else
        return Status::OK;
    }

    inline bool IsInterrupted() const
    {
      return this->IsInterrupted(0);
    }

    inline bool IsInterrupted(DWORD delay) const
    {
      if (WaitForSingleObject(hInterrupt_, delay) == WAIT_TIMEOUT)
      {
        return false;
      }
      ReleaseSemaphore(hInterrupt_, 1, NULL);  // keep interrupted state
      return true;
    }

    inline bool IsRunning() const
    {
      DWORD exitCode = 0;
      if (hThread_)
        GetExitCodeThread(hThread_, &exitCode);
      if (exitCode == STILL_ACTIVE)
        return true;
      return false;
    }

    __declspec(property(get = GetHandle)) HANDLE ThreadHandle;
    
    inline HANDLE GetHandle() const
    {
      return hThread_;
    }

    __declspec(property(get = GetID)) DWORD ThreadID;
    
    inline DWORD GetId() const
    {
      return dwThreadID_;
    }
  };

}; //namespace pbop

#endif //LIB_PBOP_THREADBUILDER