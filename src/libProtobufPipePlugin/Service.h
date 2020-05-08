#ifndef LIB_PROTOBUF_PIPE_PLUGIN_SERVICE
#define LIB_PROTOBUF_PIPE_PLUGIN_SERVICE

#include "Status.h"

#include <string>
#include <vector>

namespace libProtobufPipePlugin
{

  class Service
  {
  public:
    virtual const std::string & GetPackageName() const = 0;
    virtual const std::string & GetServiceName() const = 0;
    virtual const std::vector<std::string> & GetFunctionIdentifiers() const = 0;
    virtual Status DispatchMessage(const size_t & index, const std::string & input, std::string & output) = 0;
  };

}; //namespace libProtobufPipePlugin

#endif //LIB_PROTOBUF_PIPE_PLUGIN_SERVICE