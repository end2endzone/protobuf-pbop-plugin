#include "Status.h"

namespace libProtobufPipePlugin
{
  const Status & Status::OK = Status(STATUS_CODE_SUCCESS, "");

  Status::Status() : code_(STATUS_CODE_UNKNOWN)
  {
  }

  Status::Status(const Status & other) :
    code_(other.code_),
    message_(other.message_)
  {
  }

  Status::Status(const StatusCode & code, const std::string & message) :
    code_(code),
    message_(message)
  {
  }

  Status::~Status()
  {
  }

  void Status::SetCode(const StatusCode & c)
  {
    code_ = c;
  }

  const StatusCode & Status::GetCode() const
  {
    return code_;
  }

  void Status::SetMessage(const std::string & m)
  {
    message_ = m;
  }

  const std::string & Status::GetMessage() const
  {
    return message_;
  }

  bool Status::Success()
  {
    if (code_ == STATUS_CODE_SUCCESS)
      return true;
    return false;
  }

  void swap(Status & first, Status & second)
  {
    //https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    using std::swap; 
    swap(first.code_, second.code_);
    swap(first.message_, second.message_);
  }

  Status & Status::operator=(Status other)
  {
    //https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    swap(*this, other);
    return *this;
  }

  bool Status::operator==(const Status & other)
  {
    if (this == &other || this->code_ == other.code_)
      return true;
    return false;
  }

  bool Status::operator!=(const Status & other)
  {
    return !( (*this) == other );
  }

  Status Status::BuildOutOfMemoryStatus(const char * function)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += function;
    error_message += "': Out of memory.";
    return Status(STATUS_CODE_OUT_OF_MEMORY, error_message);
  }

  Status Status::BuildSerializationStatus(const char * function, const ::google::protobuf::Message & message)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += __FUNCTION__;
    error_message += "': failed to serialize request of type '";
    error_message += message.GetTypeName();
    error_message += "'.";
    return Status(STATUS_CODE_SERIALIZE_ERROR, error_message);
  }

  Status Status::BuildDeserializationStatus(const char * function, const ::google::protobuf::Message & message)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += __FUNCTION__;
    error_message += "': failed to deserialize request of type '";
    error_message += message.GetTypeName();
    error_message += "'.";
    return Status(STATUS_CODE_DESERIALIZE_ERROR, error_message);
  }

  Status Status::BuildMissingFieldStatus(const char * function, const char * field, const ::google::protobuf::Message & message)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += __FUNCTION__;
    error_message += "': missing status field in '";
    error_message += message.GetTypeName();
    error_message += "' message.";
    return Status(STATUS_CODE_SERIALIZE_ERROR, error_message);
  }

  Status Status::BuildNotImplementedStatus(const char * function)
  {
    std::string error_message;
    error_message += "Error function '";
    error_message += __FUNCTION__;
    error_message += "' is not implemented.";
    return Status(STATUS_CODE_NOT_IMPLEMENTED_ERROR, error_message);
  }

}; //namespace libProtobufPipePlugin
