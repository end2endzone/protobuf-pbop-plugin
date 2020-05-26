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

#include "pbop/Status.h"

namespace pbop
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

  const char * Status::ToString(const StatusCode & code)
  {
    switch (code)
    {
    case STATUS_CODE_SUCCESS:
      return "Success";
    case STATUS_CODE_SERIALIZE_ERROR:
      return "Serialize Error";
    case STATUS_CODE_DESERIALIZE_ERROR:
      return "Deserialize Error";
    case STATUS_CODE_CANCELLED:
      return "Cancelled";
    case STATUS_CODE_NOT_IMPLEMENTED:
      return "Not Implemented";
    case STATUS_CODE_INVALID_ARGUMENT:
      return "Invalid Argument";
    case STATUS_CODE_OUT_OF_RANGE:
      return "Out Of Range";
    case STATUS_CODE_OUT_OF_MEMORY:
      return "Out Of Memory";
    case STATUS_CODE_PIPE_ERROR:
      return "Pipe Error";
    case STATUS_CODE_IMPLEMENTATION:
      return "Implementation Error";
    case STATUS_CODE_UNKNOWN:
    default:
      return "Unknown";
    };
  }

  Status Status::Factory::OutOfMemory(const char * function)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += function;
    error_message += "': Out of memory.";
    return Status(STATUS_CODE_OUT_OF_MEMORY, error_message);
  }

  Status Status::Factory::Serialization(const char * function, const ::google::protobuf::Message & message)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += __FUNCTION__;
    error_message += "': failed to serialize request of type '";
    error_message += message.GetTypeName();
    error_message += "'.";
    return Status(STATUS_CODE_SERIALIZE_ERROR, error_message);
  }

  Status Status::Factory::Deserialization(const char * function, const ::google::protobuf::Message & message)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += __FUNCTION__;
    error_message += "': failed to deserialize request of type '";
    error_message += message.GetTypeName();
    error_message += "'.";
    return Status(STATUS_CODE_DESERIALIZE_ERROR, error_message);
  }

  Status Status::Factory::MissingField(const char * function, const char * field, const ::google::protobuf::Message & message)
  {
    std::string error_message;
    error_message += "Error in function '";
    error_message += __FUNCTION__;
    error_message += "': missing field '";
    error_message += field;
    error_message += "' in '";
    error_message += message.GetTypeName();
    error_message += "' message.";
    return Status(STATUS_CODE_SERIALIZE_ERROR, error_message);
  }

  Status Status::Factory::NotImplemented(const char * function)
  {
    std::string error_message;
    error_message += "Error function '";
    error_message += __FUNCTION__;
    error_message += "' is not implemented.";
    return Status(STATUS_CODE_NOT_IMPLEMENTED, error_message);
  }

}; //namespace pbop
