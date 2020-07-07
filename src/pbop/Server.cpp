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

#ifdef _WIN32
//google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
//google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)
__pragma( warning(push) )
__pragma( warning(disable: 4800))
__pragma( warning(disable: 4146))
#endif //_WIN32

#include "pbop/Server.h"
#include "pbop/Status.h"
#include "pbop/PipeConnection.h"
#include "pbop/ScopeLock.h"

#include "pbop.pb.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <Windows.h>

#include "pbop/ThreadBuilder.h"

//https://docs.microsoft.com/en-us/windows/win32/ipc/multithreaded-pipe-server

namespace pbop
{
  class Server::ClientSession
  {
  public:
    Server * server_;
    PipeConnection * connection_; //owned by the session
    connection_id_t connection_id_;
    Thread * thread_; //owned by the session

  public:
    ClientSession(Server * server,
                  connection_id_t connection_id)
    {
      connection_ = new PipeConnection();
      connection_id_ = connection_id;
      server_ = server;
      thread_ = new ThreadBuilder<ClientSession>(this, &ClientSession::Run);
    }

    ~ClientSession()
    {
      // ForceClose the connection to force session thread to exit
      if (connection_)
        connection_->ForceClose();

      if (thread_)
      {
        //make sure the thread is completed before deleting this object
        thread_->Join();

        delete thread_;
      }
      thread_ = NULL;

      if (connection_)
        delete connection_;
      connection_ = NULL;
    }

    // This routine is a thread processing function to read from and reply to a client
    // via the open pipe connection passed from the server listening loop. Note this allows
    // the main loop to continue executing, potentially creating more threads of
    // of this procedure to run concurrently, depending on the number of incoming
    // client connections.
    DWORD Run()
    {
      return server_->RunMessageProcessingLoop(this);
    }

  private:
    ClientSession(const ClientSession & copy); //disable copy constructor.
    ClientSession & operator =(const ClientSession & other); //disable assignment operator.
  };

  std::string GetErrorDesription(DWORD code);

  Server::Server() : 
    buffer_size_(10240),
    next_connection_id_(0),
    running_(false),
    shutdown_request_(false),
    shutdown_processed_(false)
  {
  }

  Server::~Server()
  {
    // Prevent other threads from manipulating services while we process this function.
    ScopeLock scope_lock(&services_cs_);

    for(size_t i=0; i<services_.size(); i++)
    {
      Service * service = services_[i];
      if (service)
        delete service;
    }
    services_.clear();
  }

  void Server::SetBufferSize(unsigned int buffer_size)
  {
    buffer_size_ = buffer_size;
  }

  unsigned int Server::GetBufferSize() const
  {
    return buffer_size_;
  }

  const char * Server::GetPipeName() const
  {
    return pipe_name_.c_str();
  }

  Status Server::Run(const char * pipe_name) 
  { 
    BOOL   fConnected = FALSE; 
    DWORD  dwThreadId = 0; 
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL; 

    pipe_name_ = pipe_name;
    running_ = true;
    shutdown_request_ = false;
    shutdown_processed_ = false;

    // Process events
    EventStartup event_startup;
    OnEvent(&event_startup);

    // The main loop creates an instance of the named pipe and 
    // then waits for a client to connect to it. When the client 
    // connects, a thread is created to handle communications 
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop until
    // a server shutdown is requested.
    while(!shutdown_request_)
    {
      hPipe = CreateNamedPipe( 
        pipe_name,                // pipe name 
        PIPE_ACCESS_DUPLEX,       // read/write access 
        PIPE_TYPE_MESSAGE |       // message type pipe 
        PIPE_READMODE_MESSAGE |   // message-read mode 
        PIPE_WAIT,                // blocking mode 
        PIPE_UNLIMITED_INSTANCES, // max. instances  
        buffer_size_,             // output buffer size 
        buffer_size_,             // input buffer size 
        0,                        // client time-out 
        NULL);                    // default security attribute 

      if (hPipe == INVALID_HANDLE_VALUE) 
      {
        std::string error_description = std::string("CreateNamedPipe failed: ") + GetErrorDesription(GetLastError());
        return Status(STATUS_CODE_PIPE_ERROR, error_description);
      }

      // Process events
      EventListening event_listening;
      OnEvent(&event_listening);

      // Wait for the client to connect; if it succeeds, 
      // the function returns a nonzero value. If the function
      // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
      fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
      if (fConnected) 
      { 
        if (!shutdown_request_)
        {
          // Process events
          next_connection_id_++;
          EventConnection event_connection;
          event_connection.SetConnectionId(next_connection_id_);
          OnEvent(&event_connection);
        }

        // Build a session for this client
        ClientSession * session = new ClientSession(this, next_connection_id_);
        session->connection_->Assign(hPipe);

        // Remember this session
        client_sessions_.push_back(session);

        // Start this session's thread.
        Status status = session->thread_->Start();
        if (!status.Success())
        {
          //Force a pipe error but keep the same error message
          status.SetCode(STATUS_CODE_PIPE_ERROR);
          return status;
        }
      }
      else
      {
        // The client could not connect, so close the pipe. 
        CloseHandle(hPipe);
      }
    } 

    // At this point, the server loop is closed.
    // There will be no new incomming pipe/connection/session.
    // Close the connections to force each session thread to exit
    for(size_t i=0; i<client_sessions_.size(); i++)
    {
      ClientSession * session = client_sessions_[i];
      session->connection_->Close();
    }

    // At this point, the listening threads should be leaving their loop.
    // There will be no new threads_.
    // Wait for all the threads to complete.
    for(size_t i=0; i<client_sessions_.size(); i++)
    {
      ClientSession * session = client_sessions_[i];
      session->thread_->Join();
    }

    // Destroy each sessions
    for(size_t i=0; i<client_sessions_.size(); i++)
    {
      ClientSession * session = client_sessions_[i];
      delete session;
    }
    client_sessions_.clear();

    // The shutdown process is completed.
    shutdown_processed_ = true;
    running_ = false;

    // Process events
    EventShutdown event_shutdown;
    OnEvent(&event_shutdown);

    return Status::OK; 
  } 

  void Server::RegisterService(Service * service)
  {
    // Prevent other threads from manipulating services while we process this function.
    ScopeLock scope_lock(&services_cs_);

    services_.push_back(service);
  }

  Status Server::RouteMessageToServiceMethod(const std::string & input, std::string & output)
  {
    // Prevent other threads from manipulating services while we process this function.
    ScopeLock scope_lock(&services_cs_);

    // Process the incoming message.
    ClientRequest client_message;
    bool success = client_message.ParseFromString(input);
    if (!success)
    {
      Status status = Status::Factory::Deserialization(__FUNCTION__, client_message);
      return status;
    }

    // Look for FunctionIdentifier
    if (!client_message.has_function_identifier())
    {
      Status status = Status::Factory::MissingField(__FUNCTION__, "function_identifier", client_message);
      return status;
    }
    const std::string & package_name = client_message.function_identifier().package();
    const std::string & service_name = client_message.function_identifier().service();
    const std::string & function_name = client_message.function_identifier().function_name();

    // Find the associated service
    Service * service = NULL;
    for(size_t i=0; i<services_.size() && service == NULL; i++)
    {
      Service * tmp = services_[i];
      if (tmp->GetPackageName() == package_name &&
          tmp->GetServiceName() == service_name)
      {
        service = tmp;
      }
    }
    if (service == NULL)
    {
      //No service
      std::string error_message;
      error_message += "Unable to dispatch message to the following service:";
      error_message += " package=" + package_name;
      error_message += " service=" + service_name;
      Status status(STATUS_CODE_NOT_IMPLEMENTED, error_message);

      return status;
    }

    // Find the index of the target function
    static const size_t INVALID_INDEX = (size_t)-1;
    size_t index = INVALID_INDEX;
    const char ** functions = service->GetFunctionIdentifiers();
    for(size_t i=0; functions != NULL && *functions != NULL && index == INVALID_INDEX && i<INVALID_INDEX; i++)
    {
      if (function_name == *functions)
        index = i;

      //search next function
      functions++;
    }

    // Did we found a valid index ?
    if (index == INVALID_INDEX)
    {
      //Not implemented
      std::string error_message;
      error_message += "Unable to dispatch message to the following function:";
      error_message += " package=" + package_name;
      error_message += " service=" + service_name;
      error_message += " function=" + function_name;
      Status status(STATUS_CODE_NOT_IMPLEMENTED, error_message);

      return status;
    }

    // Run the method
    Status status = service->InvokeMethod(index, client_message.request_buffer(), output);
    return status;
  }

  DWORD Server::RunMessageProcessingLoop(Server::ClientSession * context)
  {
    BOOL fSuccess = FALSE;

    if (!shutdown_request_)
    {
      // Process events
      EventClientCreate event_create;
      event_create.SetConnectionId(context->connection_id_);
      OnEvent(&event_create);
    }

    // Loop until done reading
    while(!shutdown_request_)
    { 
      // Read client requests from the pipe.
      std::string read_buffer;
      Status status = context->connection_->Read(read_buffer);

      // Leave the loop if a shutdown was requested 
      if (shutdown_request_)
        break;

      if (!status.Success())
      {
        DWORD wLastErrorCode = GetLastError();
        if (wLastErrorCode == ERROR_BROKEN_PIPE)
        {
          // Client disconnected

          if (!shutdown_request_)
          {
            // Process events
            EventClientDisconnected event_disconnected;
            event_disconnected.SetConnectionId(context->connection_id_);
            OnEvent(&event_disconnected);
          }
        }
        else
        {
          //other read error

          // Process events
          EventClientError event_error;
          event_error.SetConnectionId(context->connection_id_);
          event_error.SetStatus(status);
          OnEvent(&event_error);
        }
        break;
      }

      // Parse and delegate message to a service
      // This will actually call a method of a service.
      std::string * function_call_result = new std::string();
      status = this->RouteMessageToServiceMethod(read_buffer, *function_call_result);
      if (!status.Success())
      {
        delete function_call_result;
        function_call_result = NULL;

        // Process events
        EventClientError event_error;
        event_error.SetConnectionId(context->connection_id_);
        event_error.SetStatus(status);
        OnEvent(&event_error);

        break;
      }

      // Build server response for the client.
      StatusMessage * status_message = new StatusMessage();
      status_message->set_code(status.GetCode());
      status_message->set_description(status.GetMessage());

      ServerResponse server_response;
      server_response.set_allocated_status(status_message);
      server_response.set_allocated_response_buffer(function_call_result);
      
      std::string write_buffer;
      bool success = server_response.SerializeToString(&write_buffer);
      if (!success)
      {
        Status status = Status::Factory::Serialization(__FUNCTION__, server_response);

        // Process events
        EventClientError event_error;
        event_error.SetConnectionId(context->connection_id_);
        event_error.SetStatus(status);
        OnEvent(&event_error);

        break;
      }

      // Send response to client through the pipe connection.
      status = context->connection_->Write(write_buffer);
      if (!status.Success())
      {
        // Process events
        EventClientError event_error;
        event_error.SetConnectionId(context->connection_id_);
        event_error.SetStatus(status);
        OnEvent(&event_error);

        break;
      }
    }

    if (!shutdown_request_)
    {
      // Process events
      EventClientDestroy event_destroy;
      event_destroy.SetConnectionId(context->connection_id_);
      OnEvent(&event_destroy);
    }

    return 0;
  }

  bool Server::IsRunning() const
  {
    return running_;
  }

  Status Server::Shutdown()
  {
    // Prevent the Run() loop to start again
    shutdown_processed_ = false;
    shutdown_request_ = true;

    // Force a connection to the server. This will force the 
    // listening loop to verify the shutdown_request_ flag and leave.
    PipeConnection connection;
    Status status = connection.Connect(pipe_name_.c_str());
    if (!status.Success())
      return status;

    // Allow up to 5 seconds to detect the shutdown_processed_ flag
    static const size_t timeout_ms = 5000;
    for(size_t i=0; i<(timeout_ms/100) && shutdown_processed_ == false; i++)
    {
      Sleep(100);
    }

    // Validate if server loop has shutdown
    if (shutdown_processed_ == false)
      return Status(STATUS_CODE_CANCELLED, "The server shutdown was not verified. The server might still be running.");

    return Status::OK;
  }

}; //namespace pbop
