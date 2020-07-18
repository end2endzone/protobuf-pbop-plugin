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

#include "pbop/BufferedConnection.h"

namespace pbop
{
  BufferedConnection::BufferedConnection(std::string * buffer_read, std::string * buffer_write) :
    buffer_read_(buffer_read),
    buffer_write_(buffer_write)
  {
  }

  BufferedConnection::~BufferedConnection()
  {
  }

  Status BufferedConnection::Write(const std::string & buffer)
  {
    if (!buffer_write_)
      return Status(STATUS_CODE_OUT_OF_MEMORY, "Write buffer is NULL.");

    buffer_write_->append(buffer);

    return Status::OK;
  }

  Status BufferedConnection::Read(std::string & buffer)
  {
    buffer.clear();

    if (!buffer_read_)
      return Status(STATUS_CODE_OUT_OF_MEMORY, "Read buffer is NULL.");

    //copy content to output buffer
    buffer = *buffer_read_;

    //clear read buffer
    buffer_read_->clear();

    return Status::OK;
  }

  Status BufferedConnection::Read(std::string & buffer, unsigned long timeout)
  {
    return Read(buffer);
  }


}; //namespace pbop
