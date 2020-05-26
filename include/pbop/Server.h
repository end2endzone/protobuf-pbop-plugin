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

#ifndef LIB_PROTOBUF_PIPE_PLUGIN_SERVER
#define LIB_PROTOBUF_PIPE_PLUGIN_SERVER

#include "pbop/Status.h"
#include "pbop/Service.h"
#include "pbop/Connection.h"

#include <string>
#include <vector>

namespace pbop
{

  class Server
  {
  public:
    Server();
    virtual ~Server();

    virtual void SetBufferSize(unsigned int buffer_size);
    virtual unsigned int GetBufferSize() const;
    virtual Status Run(const char * pipe_name);
    virtual void RegisterService(Service * service);
    virtual unsigned long ProcessIncommingMessages(Connection * connection);

  private:
    virtual Status DispatchMessage(const std::string & input, std::string & output);
  private:
    unsigned int buffer_size_;
    std::vector<Service *> services_;
  };

}; //namespace pbop

#endif //LIB_PROTOBUF_PIPE_PLUGIN_SERVER