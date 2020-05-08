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