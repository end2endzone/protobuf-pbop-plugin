#ifndef LIB_PROTOBUF_PIPE_PLUGIN_SERVER
#define LIB_PROTOBUF_PIPE_PLUGIN_SERVER

#include "Status.h"
#include "Service.h"

#include <string>
#include <vector>

namespace libProtobufPipePlugin
{

  class Server
  {
  public:
    Server();
    virtual ~Server();

    virtual Status Run(const char * pipe_name);
    virtual void RegisterService(Service * service);
    virtual Status DispatchMessage(const std::string & input, std::string & output);
  
  private:
    std::vector<Service *> services_;
  };

}; //namespace libProtobufPipePlugin

#endif //LIB_PROTOBUF_PIPE_PLUGIN_SERVER