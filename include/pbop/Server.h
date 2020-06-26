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

#ifndef LIB_PBOP_SERVER
#define LIB_PBOP_SERVER

#include "pbop/Status.h"
#include "pbop/Service.h"
#include "pbop/Connection.h"
#include "pbop/Types.h"
#include "pbop/Events.h"

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

    virtual const char * GetPipeName() const;

    virtual Status Run(const char * pipe_name);
    virtual void RegisterService(Service * service);

    // Client threads support
    class ClientContext;
  private:
    friend class ClientContext;
    virtual unsigned long ProcessIncommingMessages(ClientContext * context);
  public:

    virtual bool IsRunning() const;

    virtual Status Shutdown();

    virtual void OnEvent(EventStartup * e) {};
    virtual void OnEvent(EventShutdown * e) {};
    virtual void OnEvent(EventListening * e) {};
    virtual void OnEvent(EventConnection * e) {};
    virtual void OnEvent(EventClientCreate * e) {};
    virtual void OnEvent(EventClientDestroy * e) {};
    virtual void OnEvent(EventClientDisconnected * e) {};
    virtual void OnEvent(EventClientError * e) {};

  private:
    virtual Status DispatchMessage(const std::string & input, std::string & output);
  private:
    std::string pipe_name_;
    unsigned int buffer_size_;
    connection_id_t next_connection_id_;
    bool running_;
    bool shutdown_request_;
    bool shutdown_processed_;
  protected:
    std::vector<Service *> services_;
    std::vector<HANDLE> pipe_handles_;
    std::vector<HANDLE> threads_;
  };

}; //namespace pbop

#endif //LIB_PBOP_SERVER