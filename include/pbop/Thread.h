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

namespace pbop
{

  /// <summary>
  /// Template class for creating threads.
  /// Inpired from https://stackoverflow.com/questions/1372967/how-do-you-use-createthread-for-functions-which-are-class-members
  /// </summary>
  template<class T>
  class Thread
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
    // -----------------------------------------------------------------------------
  private:
    /// <summary>
    /// Target function argument of the CreateThread() function.
    /// </summary>
    /// <param name="thread_obj">A pointer to this Thread object.</param>
    /// <returns>Returns the thread exit code. Returns 0 on </returns>
    static DWORD Run(LPVOID thread_obj)
    {
      Thread<T>* thread = (Thread<T>*)thread_obj;
      return (thread->object_->*thread->lpMethod_) ();
    }

    ///<summary>Disable copy constructor. Prevent copying of thread objects.</summary>
    Thread(const Thread<T>& other) {}

    ///<summary>Disable assignment operator. Prevent assignment of thread objects.</summary>
    Thread<T>& operator =(const Thread<T>& other) {}

  public:
    /// <summary>
    /// Creates a new Thread object.
    /// </summary>
    /// <param name="object">The object which owns the method that executes concurently.</param>
    /// <param name="lpMethod">The method pointer of the object of type T which is executed concurently.</param>
    explicit Thread(T* object, TMethodPointer lpMethod)
    {
      this->hThread_       = NULL;
      this->object_        = object;
      this->lpMethod_      = lpMethod;
      this->dwThreadID_    = 0;
      this->hInterrupt_    = CreateSemaphore(NULL, 1, 1, NULL);
      this->hSingleStart_  = CreateMutex(NULL, FALSE, NULL);
    }

    virtual ~Thread(void)
    {
      Join();

      if (hInterrupt_)
        CloseHandle(hInterrupt_);
      if (hSingleStart_)
        CloseHandle(hSingleStart_);
      if (hThread_)
        CloseHandle(hThread_);
    }

    /// <summary>
    /// Starts the threads by executing the method pointer of the object in a concurrent thread.
    /// </summary>
    /// <returns>Returns true if the thread was started successfully. Returns false otherwise.</returns>
    bool Start()
    {
      __try {
        if (WaitForSingleObject(hSingleStart_, 0) != WAIT_OBJECT_0)
          return false;
        if (hThread_) // The thread had been started sometime in the past
        {
          // Is the thread still running?
          if (WaitForSingleObject(hThread_, 0) == WAIT_TIMEOUT)
          {
            return false; // Deny a thread start
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
          (LPTHREAD_START_ROUTINE) Thread<T>::Run,
          this,
          0,
          &this->dwThreadID_);
        if (hThread_)
          return true;
        return false;
      }
      __finally
      {
        ReleaseMutex(hSingleStart_);
      }
    }

    /// <summary>
    /// Blocks the calling thread until this thread has stopped.
    /// </summary>
    inline void Join()
    {
      WaitForSingleObject(hThread_, INFINITE);
    }

    /// <summary>
    /// Asks the thread to exit nicely. Thread function must implement checks.
    /// </summary>
    /// <remarks>
    /// The return value indicates if the interrupt could be placed not if the thread reacts on the interrupt.
    /// </remarks>
    /// <returns>Returns true if the interrupt could be placed. Returns false otherwise.</returns>
    inline bool SetInterrupt()
    {
      if (hInterrupt_)
      {
        return ((ReleaseSemaphore(hInterrupt_, 1, NULL) == FALSE) ? false : true);
      }
      return false;
    }

    /// <summary>
    /// Returns true if an interrupt request was set. Returns false otherwise.
    /// </summary>
    /// <returns>Returns true if an interrupt request was set. Returns false otherwise.</returns>
    inline bool IsInterrupted()
    {
      return this->IsInterrupted(0);
    }

    /// <summary>
    /// Returns true if an interrupt request was set. Returns false otherwise.
    /// Waits for delay milliseconds for the interrupt to take place.
    /// </summary>
    /// <param name="delay">The maximum waiting time in milliseconds for the interrupt to take place.</param>
    /// <returns>Returns true if an interrupt request was set. Returns false otherwise.</returns>
    inline bool IsInterrupted(DWORD delay)
    {
      if (WaitForSingleObject(hInterrupt_, delay) == WAIT_TIMEOUT)
      {
        return false;
      }
      ReleaseSemaphore(hInterrupt_, 1, NULL);  // keep interrupted state
      return true;
    }

    /// <summary>
    /// Returns true if the thread is running.
    /// </summary>
    /// <returns>Returns true if the thread is running. Returns false otherwise.</returns>
    inline bool IsRunning()
    {
      DWORD exitCode = 0;
      if (hThread_)
        GetExitCodeThread(hThread_, &exitCode);
      if (exitCode == STILL_ACTIVE)
        return true;
      return false;
    }

    __declspec(property(get = GetHandle)) HANDLE ThreadHandle;
    
    /// <summary>
    /// Get the thread handle.
    /// </summary>
    /// <returns>Returns the thread handle.</returns>
    inline HANDLE GetHandle()
    {
      return hThread_;
    }

    __declspec(property(get = GetID)) DWORD ThreadID;
    
    /// <summary>
    /// Get the thread id.
    /// </summary>
    /// <returns>Returns the thread id.</returns>
    inline DWORD GetId()
    {
      return dwThreadID_;
    }
  };

}; //namespace pbop

#endif //LIB_PBOP_THREAD