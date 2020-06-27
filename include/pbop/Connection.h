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

#ifndef LIB_PBOP_CONNECTION
#define LIB_PBOP_CONNECTION

#include "pbop/Status.h"

#include <string>

namespace pbop
{

  class Connection
  {
  public:
    virtual ~Connection() {}

    /// <summary>
    /// Writes the given buffer to the connection.
    /// </summary>
    /// <param name="buffer">The buffer content to send to the connection.</param>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    virtual Status Write(const std::string & buffer) = 0;

    /// <summary>
    /// Reads an unspecified amount data from the connection.
    /// </summary>
    /// <param name="buffer">The buffer that contains the readed data.</param>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    virtual Status Read(std::string & buffer) = 0;
  };

}; //namespace pbop

#endif //LIB_PBOP_CONNECTION
