#ifndef LIB_PROTOBUF_PIPE_PLUGIN_CONNECTION
#define LIB_PROTOBUF_PIPE_PLUGIN_CONNECTION

#include "Status.h"

#include <string>
#include <vector>

namespace libProtobufPipePlugin
{

  class Connection
  {
  public:
    virtual void Assign(size_t pipe_handle) = 0;
    virtual Status Connect(const char * pipe_name) = 0;
    virtual Status Write(const std::string & message) = 0;
    virtual Status Read(std::string & message) = 0;
  };

  class PipeConnection : public Connection
  {
  public:
    PipeConnection();
    virtual ~PipeConnection();

    virtual void Assign(size_t pipe_handle);
    virtual Status Connect(const char * name);
    virtual Status Write(const std::string & message);
    virtual Status Read(std::string & message);

  private:
    PipeConnection(const PipeConnection & c);
  private:
    std::string name_;
    size_t pipe_handle_;
  };

}; //namespace libProtobufPipePlugin

#endif //LIB_PROTOBUF_PIPE_PLUGIN_CONNECTION
