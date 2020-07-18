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

#include "pbop/PipeConnection.h"

// Inspired from the following references:
//   https://docs.microsoft.com/en-us/windows/win32/ipc/named-pipe-client
//   https://docs.microsoft.com/en-us/windows/win32/fileio/canceling-pending-i-o-operations
//   https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancednamedpipe15a.html

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace pbop
{
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

  class SafeHandle
  {
  public:
    HANDLE value;

  public:
    SafeHandle() : value(NULL) {}
    SafeHandle(HANDLE hHandle) : value(hHandle) {}
    ~SafeHandle()
    {
      if (value != NULL &&                // for CreateEvent() returns NULL
          value != INVALID_HANDLE_VALUE)  // for CreateNamedPipe() returns INVALID_HANDLE_VALUE
        CloseHandle(value);
      value = NULL;
    }
  };

  const unsigned long & PipeConnection::DEFAULT_BUFFER_SIZE = 10240;

  struct PipeConnection::PImpl
  {
    HANDLE hPipe;
    HANDLE hEvent; // Handle to monitor overlapped I/O activity when using ReadFile().
  };

  PipeConnection::PipeConnection() :
    impl_(new PipeConnection::PImpl())
  {
    impl_->hPipe = INVALID_HANDLE_VALUE;
    impl_->hEvent = NULL;
  }

  PipeConnection::~PipeConnection()
  {
    Close();

    if (impl_)
      delete impl_;
    impl_ = NULL;
  }

  Status PipeConnection::Connect(const char * pipe_name)
  {
    // Disconnect from any previous pipe
    Close();

    //// Create an event handle used to monitor overlapped I/O activity on each pipe.
    //SafeHandle hEvent;
    //hEvent.value = CreateEvent(NULL, TRUE, FALSE, NULL);
    //if (hEvent.value == NULL)
    //{
    //  std::string error_description = std::string("CreateEvent failed: ") + GetErrorDesription(GetLastError());
    //  return Status(STATUS_CODE_PIPE_ERROR, error_description);
    //}

    SafeHandle hPipe;
    BOOL fSuccess = FALSE;

    // Try to open a named pipe; wait for it, if necessary. 
    while (1)
    {
      hPipe.value = CreateFile(
          pipe_name,      // pipe name
          GENERIC_READ |  // read and write access
          GENERIC_WRITE,
          0,              // no sharing
          NULL,           // default security attributes
          OPEN_EXISTING,  // opens existing pipe
          0,              // default attributes
          NULL);          // no template file
 
      // Break if the pipe handle is valid.
      if (hPipe.value != INVALID_HANDLE_VALUE)
          break;
 
      // Exit if an error other than ERROR_PIPE_BUSY occurs.
      DWORD wLastError = GetLastError();
      if (wLastError != ERROR_PIPE_BUSY)
      {
        // Error, unable to connect
        std::string error_description = GetErrorDesription(wLastError);
        return Status(STATUS_CODE_PIPE_ERROR, error_description);
      }
 
      // All pipe instances are busy, so wait for 20 seconds.
      if (!WaitNamedPipe(pipe_name, 20000))
      {
        std::string error_description = "Could not open pipe: 20 second wait timed out.";
        return Status(STATUS_CODE_PIPE_ERROR, error_description);
      }
    }
 
    // The pipe connected; change to message-read mode.
    DWORD dwMode = PIPE_READMODE_MESSAGE;
    fSuccess = SetNamedPipeHandleState(
      hPipe.value,  // pipe handle
      &dwMode,      // new pipe mode
      NULL,         // don't set maximum bytes
      NULL);        // don't set maximum time
    if (!fSuccess)
    {
      std::string error_description = std::string("SetNamedPipeHandleState failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    // Connection successful
    impl_->hPipe = hPipe.value;
    //impl_->hEvent = hEvent.value;
    name_ = pipe_name;

    // Detach created handles
    hPipe.value = NULL;
    //hEvent.value = NULL;

    return Status::OK;
  }

  Status PipeConnection::Write(const std::string & buffer)
  {
    if (impl_->hPipe == INVALID_HANDLE_VALUE)
      return Status(STATUS_CODE_PIPE_ERROR, "Pipe is invalid.");

    DWORD wBytesWritten = 0;
    BOOL fSuccess = WriteFile(
      impl_->hPipe,           // pipe handle
      buffer.data(),          // message
      buffer.size(),          // message length
      &wBytesWritten,         // bytes written
      NULL);                  // not overlapped

    if (!fSuccess || wBytesWritten != buffer.size())
    {
      std::string error_description = std::string("WriteFile to pipe failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    return Status::OK;
  }

  Status PipeConnection::Read(std::string & buffer)
  {
    buffer.clear();

    if (impl_->hPipe == INVALID_HANDLE_VALUE)
      return Status(STATUS_CODE_PIPE_ERROR, "Pipe is invalid.");

    BOOL fSuccess = FALSE;
    static const size_t BUFFER_SIZE = 10240;
    char tmp[BUFFER_SIZE];
    do
    {
      // Read from the pipe.
      DWORD wBytesReaded = 0;
      fSuccess = ReadFile(
          impl_->hPipe, // pipe handle
          tmp,          // buffer to receive reply
          BUFFER_SIZE,  // size of buffer
          &wBytesReaded,// number of bytes read
          NULL);        // not overlapped

      if (fSuccess)
        buffer.append(tmp, wBytesReaded);
      else
      {
        const DWORD dwLastError = GetLastError();
        if ( dwLastError == ERROR_MORE_DATA )
        {
          // Keep what we already read and run another loop
          buffer.append(tmp, wBytesReaded);
        }
        else
          break; // Something unexpected happend. That is an actual error
      }
 
      // Received message is incomplete.
      // Loop again

    } while (!fSuccess); // Repeat loop if ERROR_MORE_DATA 

    if (!fSuccess)
    {
      std::string error_description = std::string("ReadFile from pipe failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    return Status::OK;
  }

  Status PipeConnection::Read(std::string & buffer, unsigned long timeout)
  {
    buffer.clear();

    if (impl_->hPipe == INVALID_HANDLE_VALUE)
      return Status(STATUS_CODE_PIPE_ERROR, "Pipe is invalid.");
    if (impl_->hEvent == NULL)
      return Status(STATUS_CODE_PIPE_ERROR, "Pipe event is not initialized.");

    OVERLAPPED overlapped = {0};
    overlapped.hEvent = impl_->hEvent;

#ifdef _DEBUG
    const DWORD eERROR_MORE_DATA = ERROR_MORE_DATA;
    const DWORD eERROR_IO_PENDING = ERROR_IO_PENDING;
    const DWORD eERROR_IO_INCOMPLETE = ERROR_IO_INCOMPLETE;
    const DWORD eERROR_OPERATION_ABORTED = ERROR_OPERATION_ABORTED;
#endif

    BOOL fSuccess = FALSE;
    DWORD dwLastError = 0;
    static const size_t BUFFER_SIZE = 10240;
    char tmp[BUFFER_SIZE];
    do
    {
      // Read from the pipe.
      DWORD wBytesReaded = 0;
      fSuccess = ReadFile(
          impl_->hPipe, // pipe handle
          tmp,          // buffer to receive reply
          BUFFER_SIZE,  // size of buffer
          &wBytesReaded,// number of bytes read
          &overlapped); // using asynchronous read

      if (fSuccess)
      {
        // The read operation is completed.
        buffer.append(tmp, wBytesReaded);
      }
      else
      {
        dwLastError = GetLastError();
        if ( dwLastError == ERROR_MORE_DATA )
        {
          // Get the actual readed size.
          // GetOverlappedResult() is expected to return false and
          // GetLastError() still set to ERROR_MORE_DATA
          if (GetOverlappedResult(impl_->hPipe, &overlapped, &wBytesReaded, FALSE) != FALSE && GetLastError() != ERROR_MORE_DATA)
          {
            std::string error_description = std::string("ReadFile, GetOverlappedResult failed: ") + GetErrorDesription(GetLastError());
            return Status(STATUS_CODE_PIPE_ERROR, error_description);
          }

          // Keep what we already read and run another loop
          buffer.append(tmp, wBytesReaded);
        }
        else if (dwLastError == ERROR_IO_PENDING)
        {
          // The I/O is pending, wait and see if the call completes in the allowed time or times out.
          if (WaitForSingleObject(overlapped.hEvent, timeout) == WAIT_FAILED)
          {
            std::string error_description = std::string("ReadFile, WaitForSingleObject failed: ") + GetErrorDesription(GetLastError());
            return Status(STATUS_CODE_PIPE_ERROR, error_description);
          }

          ResetEvent(impl_->hEvent);

          // At this point, the ReadFile() operation has completed or 
          // has timed out (according to our timeout value)
          fSuccess = GetOverlappedResult(impl_->hPipe, &overlapped, &wBytesReaded, FALSE);
          if (fSuccess)
          {
            // The read operation is completed without having to wait too much.
            buffer.append(tmp, wBytesReaded);
          }
          else
          {
            dwLastError = GetLastError();
            if ( dwLastError == ERROR_MORE_DATA )
            {
              // Keep what we already read and run another loop
              buffer.append(tmp, wBytesReaded);
            }
            else if (dwLastError == ERROR_IO_PENDING || dwLastError == ERROR_IO_INCOMPLETE)
            {
              // The read is still pending even after waiting for our timeout value
              // Try to cancel the read to return a timeout status.
              if (CancelIoEx( impl_->hPipe, &overlapped ) == 0)
              {
                std::string error_description = std::string("ReadFile, CancelIoEx failed: ") + GetErrorDesription(GetLastError());
                return Status(STATUS_CODE_PIPE_ERROR, error_description);
              }

              // The cancel request is accepted by the subsystem driver.
              // Wait for the I/O subsystem to acknowledge our cancellation.
              // Depending on the timing of the calls, the I/O might complete with a
              // cancellation status, or it might complete normally (if the ReadFile was
              // in the process of completing at the time CancelIoEx was called, or if
              // the device does not support cancellation).
              // This call specifies TRUE for the bWait parameter, which will block
              // until the I/O either completes or is canceled, thus resuming execution, 
              // provided the underlying device driver and associated hardware are functioning 
              // properly. If there is a problem with the driver it is better to stop 
              // responding here than to try to continue while masking the problem.

              fSuccess = GetOverlappedResult(impl_->hPipe, &overlapped, &wBytesReaded, TRUE);
              if (fSuccess)
              {
                // The read operation is completed before the cancell request.
                buffer.append(tmp, wBytesReaded);
              }
              else
              {
                dwLastError = GetLastError();
                if ( dwLastError == ERROR_MORE_DATA )
                {
                  // Keep what we already read and run another loop
                  buffer.append(tmp, wBytesReaded);
                }
                else if (dwLastError == ERROR_OPERATION_ABORTED)
                {
                  std::string error_description = std::string("Read() has timed out");
                  return Status(STATUS_CODE_TIMED_OUT, error_description);
                }
                else
                  break; //something unexpected happend.
              }
            }
            else
              break; //something unexpected happend.
          }
        }
        else
          break; //something unexpected happend.
      }

      // Received message is incomplete.
      // Loop again

    } while (!fSuccess); // Repeat loop if ERROR_MORE_DATA 

    if (!fSuccess)
    {
      std::string error_description = std::string("ReadFile from pipe failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    return Status::OK;
  }

  void PipeConnection::Close()
  {
    if (impl_->hEvent != NULL)
      CloseHandle(impl_->hEvent);
    impl_->hEvent = NULL;

    if (impl_->hPipe != INVALID_HANDLE_VALUE)
    {
      // Flush the pipe to allow the client to read the pipe's contents 
      // before disconnecting. Then disconnect the pipe, and close the 
      // handle to this pipe instance.
      FlushFileBuffers(impl_->hPipe); 
      DisconnectNamedPipe(impl_->hPipe); 

      CloseHandle(impl_->hPipe);
    }
    impl_->hPipe = INVALID_HANDLE_VALUE;
  }

  void PipeConnection::ForceClose()
  {
    // Make this instance forget about its internal pipe handle
    HANDLE hPipe = impl_->hPipe;
    impl_->hPipe = INVALID_HANDLE_VALUE; // This should be an atomic call

    // Close the handle. This should unblock a Read() call. ReadFile() function should return an error.
    if (hPipe != INVALID_HANDLE_VALUE)
    {
      // Flush the pipe to allow the client to read the pipe's contents 
      // before disconnecting. Then disconnect the pipe, and close the 
      // handle to this pipe instance.
      FlushFileBuffers(hPipe); 
      DisconnectNamedPipe(hPipe); 

      CloseHandle(hPipe);
    }
  }

  Status PipeConnection::Listen(const char * pipe_name, PipeConnection ** connection, ListenOptions * options)
  {
    if (connection == NULL)
      return Status(STATUS_CODE_INVALID_ARGUMENT, "Argument 'connection' is NULL");
    *connection = NULL;

    // Handle buffer size option
    DWORD dwOutBufferSize = DEFAULT_BUFFER_SIZE;
    DWORD dwInBufferSize  = DEFAULT_BUFFER_SIZE;
    if (options)
    {
      dwOutBufferSize = options->buffer_size;
      dwInBufferSize  = options->buffer_size;
    }

    // Create an event handle used to monitor overlapped I/O activity on each pipe.
    SafeHandle hEvent;
    hEvent.value = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent.value == NULL)
    {
      std::string error_description = std::string("CreateEvent failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    // Create an instance of the named pipe
    SafeHandle hPipe;
    hPipe.value = CreateNamedPipe(
      pipe_name,                // pipe name
      PIPE_ACCESS_DUPLEX |      // read/write access
      FILE_FLAG_OVERLAPPED,     // use overlapped mode,
      PIPE_TYPE_MESSAGE |       // message type pipe
      PIPE_READMODE_MESSAGE |   // message-read mode
      PIPE_WAIT,                // blocking mode
      PIPE_UNLIMITED_INSTANCES, // max. instances
      dwOutBufferSize,          // output buffer size
      dwInBufferSize,           // input buffer size
      0,                        // client time-out
      NULL);                    // default security attribute

    if (hPipe.value == INVALID_HANDLE_VALUE)
    {
      std::string error_description = std::string("CreateNamedPipe failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    // Wait for the client to connect; if it succeeds,
    // the function returns a nonzero value. If the function
    // returns zero, GetLastError returns ERROR_PIPE_CONNECTED.
    BOOL bConnected = ConnectNamedPipe(hPipe.value, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (bConnected)
    {
      // Build a PipeConnection that wraps this HANDLE
      *connection = new PipeConnection();
      (*connection)->impl_->hPipe = hPipe.value;
      (*connection)->impl_->hEvent = hEvent.value;

      // Detach handles to prevent destroying when retuning.
      hPipe.value = NULL;
      hEvent.value = NULL;
    }
    else
    {
      std::string error_description = std::string("ConnectNamedPipe failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    return Status::OK;
  }

}; //namespace pbop
