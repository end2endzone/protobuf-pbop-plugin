#ifndef LIB_PROTOBUF_PIPE_PLUGIN_STATUS
#define LIB_PROTOBUF_PIPE_PLUGIN_STATUS

#include <string>

#include <google/protobuf/message.h>

namespace libProtobufPipePlugin
{

  enum StatusCode
  {
    STATUS_CODE_SUCCESS,
    STATUS_CODE_UNKNOWN,
    STATUS_CODE_NOT_IMPLEMENTED,
    STATUS_CODE_OUT_OF_MEMORY,
    STATUS_CODE_SERIALIZE_ERROR,
    STATUS_CODE_DESERIALIZE_ERROR,
    STATUS_CODE_PIPE_ERROR,
  };

  class Status
  {
  public:
    Status();
    Status(const Status & other);
    Status(const StatusCode & code, const std::string & message);
    ~Status();

    void SetCode(const StatusCode & c);
    const StatusCode & GetCode() const;

    void SetMessage(const std::string & m);
    const std::string & GetMessage() const;

    bool Success();

    friend void swap(Status & first, Status & second);
    Status & operator=(Status other);
    bool operator==(const Status & other);
    bool operator!=(const Status & other);

    static const Status & OK;

    class Factory
    {
    public:
      static Status OutOfMemory(const char * function);
      static Status Serialization(const char * function, const ::google::protobuf::Message & message);
      static Status Deserialization(const char * function, const ::google::protobuf::Message & message);
      static Status MissingField(const char * function, const char * field, const ::google::protobuf::Message & message);
      static Status NotImplemented(const char * function);
    };

  private:
    StatusCode code_;
    std::string message_;
  };

}; //namespace libProtobufPipePlugin

#endif //LIB_PROTOBUF_PIPE_PLUGIN_STATUS