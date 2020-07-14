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

#ifndef LIB_PBOP_PIPE_CONNECTION
#define LIB_PBOP_PIPE_CONNECTION

#include "pbop/Status.h"
#include "pbop/Connection.h"

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

namespace pbop
{

  class PipeConnection : public Connection
  {

  public:
    PipeConnection();
    virtual ~PipeConnection();
  private:
    PipeConnection(const PipeConnection & copy); //disable copy constructor.
    PipeConnection & operator =(const PipeConnection & other); //disable assignment operator.
  public:

    /// <summary>The default reading and writing buffer size in bytes.</summary>
    static const unsigned long & DEFAULT_BUFFER_SIZE;

    virtual Status Write(const std::string & buffer);
    virtual Status Read(std::string & buffer);

    /// <summary>
    /// Assigns an already connected pipe HANDLE to this connection.
    /// This instance takes ownership of the given HANDLE.
    /// If an existing handle is already assigned to the connection, 
    /// the existing connection will be closed and the new handle will be assigned to this connection.
    /// </summary>
    /// <param name="hPipe">An valid pipe HANDLE.</param>
    virtual void Assign(HANDLE hPipe);

    /// <summary>
    /// Initiate a pipe connection to the given pipe name.
    /// </summary>
    /// <param name="name">A valid pipe name (path). On Windows, pipe names must be in the following format: \\.\pipe\[name] where [name] is an actual file.</param>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    virtual Status Connect(const char * name);

    /// <summary>
    /// Close the connection.
    /// </summary>
    virtual void Close();

    /// <summary>
    /// Force closes the connection.
    /// This is required if a process is blocked in a Read() call
    /// but the connection must be closed.
    /// </summary>
    virtual void ForceClose();

    /// <summary>The list of configuration options while listening for an incomming pipe connection.</summary>
    struct ListenOptions
    {
      unsigned long buffer_size; // Size of the reading and writing buffers in bytes. Set to DEFAULT_BUFFER_SIZE for default value.
    };

    /// <summary>
    /// Creates an instance of the named pipe and wait for a client to connect to it.
    /// </summary>
    /// <param name="name">A valid pipe name (path). On Windows, pipe names must be in the following format: \\.\pipe\[name] where [name] is an actual file.</param>
    /// <param name="connection">An output pointer to PipeConnection. On success, the pointer is set to a new PipeConection instance. On failure, the pointer is set to NULL.</param>
    /// <param name="options">A pointer to a ListenOptions structure for configuring the behavior of the Listen function. Set to NULL for default options.</param>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    static Status Listen(const char * pipe_name, PipeConnection ** connection, ListenOptions * options);

  private:
    std::string name_;
    HANDLE hPipe_;
  };

}; //namespace pbop

#endif //LIB_PBOP_PIPE_CONNECTION
