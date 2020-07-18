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
#include "pbop/Thread.h"
#include "pbop/CriticalSection.h"

#include <string>
#include <vector>

namespace pbop
{

  class Server
  {

  public:
    Server();
    virtual ~Server();
  private:
    Server(const Server & copy); //disable copy constructor.
    Server & operator =(const Server & other); //disable assignment operator.
  public:

    /// <summary>The default reading and writing buffer size in bytes.</summary>
    static const unsigned long & DEFAULT_BUFFER_SIZE;

    /// <summary>The default reading timeout time in milliseconds.</summary>
    static const unsigned long & DEFAULT_TIMEOUT_TIME;

    virtual void SetBufferSize(unsigned int buffer_size);
    virtual unsigned int GetBufferSize() const;

    virtual const char * GetPipeName() const;

    virtual Status Run(const char * pipe_name);
    virtual void RegisterService(Service * service);

    // Threads support for client connections
    class ClientSession;
  private:
    friend class ClientSession;
    virtual unsigned long RunMessageProcessingLoop(ClientSession * context);
    virtual Status RouteMessageToServiceMethod(const std::string & input, std::string & output);
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
    std::string pipe_name_;
    unsigned int buffer_size_;
    connection_id_t next_connection_id_;
    bool running_;
    volatile bool shutdown_request_;
    volatile bool shutdown_processed_;
  protected:
    std::vector<Service *> services_;
    std::vector<ClientSession *> client_sessions_;
    CriticalSection services_cs_;
  };

}; //namespace pbop

#endif //LIB_PBOP_SERVER