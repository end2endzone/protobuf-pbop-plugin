#ifndef LIB_PROTOBUF_PIPE_PLUGIN_CONNECTION
#define LIB_PROTOBUF_PIPE_PLUGIN_CONNECTION

#include "libProtobufPipePlugin/Status.h"

#include <string>
#include <vector>

namespace libProtobufPipePlugin
{

  class Connection
  {
  public:
    virtual Status Write(const std::string & buffer) = 0;
    virtual Status Read(std::string & buffer) = 0;
  };

  class PipeConnection : public Connection
  {
  public:
    PipeConnection();
    virtual ~PipeConnection();

    virtual Status Write(const std::string & buffer);
    virtual Status Read(std::string & buffer);

    virtual void Assign(size_t pipe_handle);
    virtual Status Connect(const char * name);

  private:
    PipeConnection(const PipeConnection & c);
  private:
    std::string name_;
    size_t pipe_handle_;
  };

}; //namespace libProtobufPipePlugin

#endif //LIB_PROTOBUF_PIPE_PLUGIN_CONNECTION
