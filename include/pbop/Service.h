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

#ifndef LIB_PROTOBUF_PIPE_PLUGIN_SERVICE
#define LIB_PROTOBUF_PIPE_PLUGIN_SERVICE

#include "Status.h"

#include <string>
#include <vector>

namespace pbop
{

  class Service
  {
  public:
    virtual const std::string & GetPackageName() const = 0;
    virtual const std::string & GetServiceName() const = 0;
    virtual const std::vector<std::string> & GetFunctionIdentifiers() const = 0;
    virtual Status DispatchMessage(const size_t & index, const std::string & input, std::string & output) = 0;
  };

}; //namespace pbop

#endif //LIB_PROTOBUF_PIPE_PLUGIN_SERVICE