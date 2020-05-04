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
    STATUS_CODE_OUT_OF_MEMORY,
    STATUS_CODE_SERIALIZE_ERROR,
    STATUS_CODE_DESERIALIZE_ERROR,
    STATUS_CODE_PIPE_ERROR,
    STATUS_CODE_NOT_IMPLEMENTED_ERROR,
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

    static Status BuildOutOfMemoryStatus(const char * function);
    static Status BuildSerializationStatus(const char * function, const ::google::protobuf::Message & message);
    static Status BuildDeserializationStatus(const char * function, const ::google::protobuf::Message & message);
    static Status BuildMissingFieldStatus(const char * function, const char * field, const ::google::protobuf::Message & message);
    static Status BuildNotImplementedStatus(const char * function);

  private:
    StatusCode code_;
    std::string message_;
  };

}; //namespace libProtobufPipePlugin

#endif //LIB_PROTOBUF_PIPE_PLUGIN_STATUS