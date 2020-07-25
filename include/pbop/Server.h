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
#include "pbop/ReadWriteLock.h"

#include <string>
#include <vector>

namespace pbop
{

  /// <summary>
  /// A pipe server that handles communication from clients.
  /// </summary>
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

    /// <summary>
    /// Set the send and receive buffers size of the connection
    /// </summary>
    /// <param name="buffer_size">The size of the buffers in bytes.</param>
    virtual void SetBufferSize(unsigned int buffer_size);

    /// <summary>
    /// Get the send and receive buffers size of the connection
    /// </summary>
    /// <returns>Returns the send and receive buffers size of the connection.</returns>
    virtual unsigned int GetBufferSize() const;

    /// <summary>
    /// Get the pipe name use with the Run() command.
    /// </summary>
    /// <returns>Returns the pipe name use with the Run() command.</returns>
    virtual const char * GetPipeName() const;

    /// <summary>
    /// Run the server and monitors incomming connections.
    /// The function is blocking until Shutdown() function is called.
    /// </summary>
    /// <param name="pipe_name">The pipe name used for listening for incomming connection.</param>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    virtual Status Run(const char * pipe_name);

    /// <summary>
    /// Register a service implementation to the server.
    /// The server takes ownership of the service instance.
    /// </summary>
    /// <param name="service">A valid service instance.</param>
    virtual void RegisterService(Service * service);

    // Threads support for client connections
    class ClientSession;
  private:
    friend class ClientSession;
    virtual unsigned long RunMessageProcessingLoop(ClientSession * context);
    virtual Status RouteMessageToServiceMethod(const std::string & input, std::string & output);
  public:

    /// <summary>
    /// Returns true if the server is running the Run() method.
    /// </summary>
    /// <returns>Returns true if the server is running the Run() method. Returns false otherwise.</returns>
    virtual bool IsRunning() const;

    /// <summary>
    /// Shut down the server. This forces the Run() method to exit gracefully.
    /// The shutdown process may fail if Client Session are still active after the shut down signal is sent to the server.
    /// </summary>
    /// <returns>Returns a Status instance which code is set to STATUS_CODE_SUCCESS when the operation is successful.</returns>
    virtual Status Shutdown();

    /// <summary>Callback function for the event that is published when the server starting up.</summary>
    virtual void OnEvent(EventStartup * e) {};

    /// <summary>Callback function for the event that is published when the server is shutting down.</summary>
    virtual void OnEvent(EventShutdown * e) {};

    /// <summary>Callback function for the event that is published when the server starts listening for incomming connections.</summary>
    virtual void OnEvent(EventListening * e) {};

    /// <summary>Callback function for the event that is published when a client has connected.</summary>
    virtual void OnEvent(EventConnection * e) {};

    /// <summary>Callback function for the event that is published when a client session is created.</summary>
    virtual void OnEvent(EventClientCreate * e) {};

    /// <summary>Callback function for the event that is published when a client session is destroyed.</summary>
    virtual void OnEvent(EventClientDestroy * e) {};

    /// <summary>Callback function for the event that is published when a client session is disconnected.</summary>
    virtual void OnEvent(EventClientDisconnected * e) {};

    /// <summary>Callback function for the event that is published when a client session has encountered an error.</summary>
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
    ReadWriteLock services_lock_;
  };

}; //namespace pbop

#endif //LIB_PBOP_SERVER