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

    virtual Status Write(const std::string & buffer);
    virtual Status Read(std::string & buffer);

    /// <summary>
    /// Assigns an alreay connected pipe HANDLE to this connection.
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

  private:
    PipeConnection(const PipeConnection & c);
  private:
    std::string name_;
    HANDLE hPipe_;
  };

}; //namespace pbop

#endif //LIB_PBOP_PIPE_CONNECTION
