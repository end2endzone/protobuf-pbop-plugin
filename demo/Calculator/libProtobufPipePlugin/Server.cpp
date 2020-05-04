#include "Server.h"
#include "Status.h"
#include "Connection.h"
#include "PipeMessages.pb.h"

//#include <strsafe.h>
#include <Windows.h>
#undef GetMessage
#undef DispatchMessage

//https://docs.microsoft.com/en-us/windows/win32/ipc/multithreaded-pipe-server

#define BUFSIZE 4096

namespace libProtobufPipePlugin
{
  DWORD WINAPI InstanceThread(LPVOID);
  std::string GetErrorDesription(DWORD code);

  struct ThreadParams
  {
    Server * server;
    HANDLE pipe;
  };

  Server::Server()
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
        BUFSIZE,                  // output buffer size 
        BUFSIZE,                  // input buffer size 
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

        // Create a thread for this client. 
        hThread = CreateThread( 
          NULL,              // no security attribute 
          0,                 // default stack size 
          InstanceThread,    // thread proc
          (LPVOID) hPipe,    // thread parameter 
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
      Status status = Status::BuildDeserializationStatus(__FUNCTION__, client_message);
      return status;
    }

    // Look for FunctionIdentifier
    if (!client_message.has_function_identifier())
    {
      Status status = Status::BuildMissingFieldStatus(__FUNCTION__, "function_identifier", client_message);
      return status;
    }
    const std::string & package_name = client_message.function_identifier().package();
    const std::string & service_name = client_message.function_identifier().service();
    const std::string & function_name = client_message.function_identifier().function_name();

    // Find the associated service
    Service * service = NULL;
    for(size_t i=0; i<services_.size(); i++)
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
      Status status(STATUS_CODE_NOT_IMPLEMENTED_ERROR, error_message);

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
      Status status(STATUS_CODE_NOT_IMPLEMENTED_ERROR, error_message);

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
    BOOL fSuccess = FALSE;
    HANDLE hPipe = NULL;
    Status status;

    // Print verbose messages. In production code, this should be for debugging only.
    printf("InstanceThread created, receiving and processing messages.\n");

    // Read the thread's parameters
    ThreadParams * thread_params = static_cast<ThreadParams*>(lpvParam);

    // Create a connection for this pipe
    PipeConnection connection;
    connection.Assign(reinterpret_cast<size_t>(thread_params->pipe));

    // Loop until done reading
    while (1) 
    { 
      // Read client requests from the pipe.
      std::string read_buffer;
      status = connection.Read(read_buffer);
      if (!status.Success())
      {
        DWORD wLastErrorCode = GetLastError();
        if (wLastErrorCode == ERROR_BROKEN_PIPE)
        {
          printf("InstanceThread: client disconnected.\n");
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
      status = thread_params->server->DispatchMessage(read_buffer, *function_call_result);
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
        Status status = Status::BuildSerializationStatus(__FUNCTION__, server_response);
        printf("InstanceThread: %d, %s\n", status.GetCode(), status.GetMessage().c_str());
        break;
      }

      // Send response to client through the pipe connection.
      Status status = connection.Write(write_buffer);
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

    delete thread_params;

    printf("InstanceThread exiting.\n");
    return 1;
  }

}; //namespace libProtobufPipePlugin
