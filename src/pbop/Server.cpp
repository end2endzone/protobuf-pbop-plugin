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
#include "pbop/Connection.h"

#include "PipeMessages.pb.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <Windows.h>
#undef GetMessage
#undef DispatchMessage

//https://docs.microsoft.com/en-us/windows/win32/ipc/multithreaded-pipe-server

namespace pbop
{
  DWORD WINAPI InstanceThread(LPVOID);
  std::string GetErrorDesription(DWORD code);

  struct ThreadParams
  {
    Server * server;
    HANDLE pipe;
  };

  Server::Server() : buffer_size_(10240)
  {
  }

  Server::~Server()
  {
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

  Status Server::Run(const char * pipe_name) 
  { 
    BOOL   fConnected = FALSE; 
    DWORD  dwThreadId = 0; 
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL; 

    // The main loop creates an instance of the named pipe and 
    // then waits for a client to connect to it. When the client 
    // connects, a thread is created to handle communications 
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop.
    for (;;) 
    { 
      printf("\nPipe Server: Main thread awaiting client connection on %s\n", pipe_name);
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

      // Wait for the client to connect; if it succeeds, 
      // the function returns a nonzero value. If the function
      // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

      fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
      if (fConnected) 
      { 
        printf("Client connected, creating a processing thread.\n"); 

        ThreadParams * thread_params = new ThreadParams();
        thread_params->pipe = hPipe;
        thread_params->server = this;

        // Create a thread for this client. 
        hThread = CreateThread( 
          NULL,              // no security attribute 
          0,                 // default stack size 
          InstanceThread,    // thread proc
          thread_params,     // thread parameter 
          0,                 // not suspended 
          &dwThreadId);      // returns thread ID 

        if (hThread == NULL) 
        {
          std::string error_description = std::string("CreateThread failed: ") + GetErrorDesription(GetLastError());
          return Status(STATUS_CODE_PIPE_ERROR, error_description);
        }
        else
          CloseHandle(hThread); 
      } 
      else
      {
        // The client could not connect, so close the pipe. 
        CloseHandle(hPipe);
      }
    } 

    return Status::OK; 
  } 

  void Server::RegisterService(Service * service)
  {
    services_.push_back(service);
  }

  Status Server::DispatchMessage(const std::string & input, std::string & output)
  {
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
    const std::vector<std::string> & functions = service->GetFunctionIdentifiers();
    for(size_t i=0; i<functions.size() && index == INVALID_INDEX; i++)
    {
      const std::string & tmp_function_name = functions[i];
      if (tmp_function_name == function_name)
        index = i;
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
    Status status = service->DispatchMessage(index, client_message.request_buffer(), output);
    return status;
  }

  // This routine is a thread processing function to read from and reply to a client
  // via the open pipe connection passed from the main loop. Note this allows
  // the main loop to continue executing, potentially creating more threads of
  // of this procedure to run concurrently, depending on the number of incoming
  // client connections.
  DWORD WINAPI InstanceThread(LPVOID lpvParam)
  {
    // Read the thread's parameters
    ThreadParams * thread_params = static_cast<ThreadParams*>(lpvParam);

    // Copy ThreadParams locally
    Server * server = thread_params->server;
    HANDLE hPipe = thread_params->pipe;
    delete thread_params;

    // Create a connection for this thread
    PipeConnection * connection = new PipeConnection();
    connection->Assign(reinterpret_cast<size_t>(hPipe));

    // Continue with the server for processing messages
    return server->ProcessIncommingMessages(connection);
  }

  DWORD Server::ProcessIncommingMessages(Connection * connection)
  {
    BOOL fSuccess = FALSE;
    HANDLE hPipe = NULL;

    // Print verbose messages. In production code, this should be for debugging only.
    printf("InstanceThread: Created, receiving and processing messages.\n");

    // Loop until done reading
    while (1) 
    { 
      // Read client requests from the pipe.
      std::string read_buffer;
      Status status = connection->Read(read_buffer);
      if (!status.Success())
      {
        DWORD wLastErrorCode = GetLastError();
        if (wLastErrorCode == ERROR_BROKEN_PIPE)
        {
          printf("InstanceThread: Client disconnected.\n");
        }
        else
        {
          //other read error
          printf("InstanceThread: %d, %s\n", status.GetCode(), status.GetMessage().c_str());
        }
        break;
      }

      // Parse and delegate message to a service
      // This will actually call a method of a service.
      std::string * function_call_result = new std::string();
      status = this->DispatchMessage(read_buffer, *function_call_result);
      if (!status.Success())
      {
        delete function_call_result;
        function_call_result = NULL;
        printf("InstanceThread: %d, %s\n", status.GetCode(), status.GetMessage().c_str());
        break;
      }

      // Build server response for the client.
      ServerStatus * server_status = new ServerStatus();
      server_status->set_code(status.GetCode());
      server_status->set_allocated_description(new std::string(status.GetMessage()));

      ServerResponse server_response;
      server_response.set_allocated_status(server_status);
      server_response.set_allocated_response_buffer(function_call_result);
      
      std::string write_buffer;
      bool success = server_response.SerializeToString(&write_buffer);
      if (!success)
      {
        Status status = Status::Factory::Serialization(__FUNCTION__, server_response);
        printf("InstanceThread: %d, %s\n", status.GetCode(), status.GetMessage().c_str());
        break;
      }

      // Send response to client through the pipe connection.
      status = connection->Write(write_buffer);
      if (!status.Success())
      {
        printf("InstanceThread: %d, %s\n", status.GetCode(), status.GetMessage().c_str());
        break;
      }
    }

    // Flush the pipe to allow the client to read the pipe's contents 
    // before disconnecting. Then disconnect the pipe, and close the 
    // handle to this pipe instance. 

    FlushFileBuffers(hPipe); 
    DisconnectNamedPipe(hPipe); 
    //CloseHandle(hPipe); will be processed by PipeConnection destructor

    delete connection;

    printf("InstanceThread: Exiting.\n");
    return 1;
  }

}; //namespace pbop
