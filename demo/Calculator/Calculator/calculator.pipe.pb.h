// Generated by the protocol buffer pipe pluging.  DO NOT EDIT!
// source: calculator.proto

#ifndef PROTOBUF_CALCULATOR_PIPE_H
#define PROTOBUF_CALCULATOR_PIPE_H

#include "calculator.pb.h"

#include "libProtobufPipePlugin/Status.h"
#include "libProtobufPipePlugin/Service.h"
#include "libProtobufPipePlugin/Connection.h"

#include <string>

namespace calculus
{

  namespace CalculatorService
  {
    class CalculatorService
    {
      virtual libProtobufPipePlugin::Status Add(const AddRequest & request, AddResponse & response) = 0;
    };

    class Client : public virtual CalculatorService
    {
    public:
      Client(libProtobufPipePlugin::Connection * connection);
      virtual ~Client();
      virtual libProtobufPipePlugin::Status Add(const AddRequest & request, AddResponse & response);
    private:
      libProtobufPipePlugin::Status ProcessCall(const char * name, const ::google::protobuf::Message & request, ::google::protobuf::Message & response);
      libProtobufPipePlugin::Connection * connection_;
    };

    class ServerStub : public virtual CalculatorService, public virtual libProtobufPipePlugin::Service
    {
    public:
      ServerStub();
      virtual ~ServerStub();

      //libProtobufPipePlugin::Service definition
      virtual const std::string & GetPackageName() const;
      virtual const std::string & GetServiceName() const;
      virtual const std::vector<std::string> & GetFunctionIdentifiers() const;
      virtual libProtobufPipePlugin::Status DispatchMessage(const size_t & index, const std::string & input, std::string & output);

      //CalculatorService implementation
      inline libProtobufPipePlugin::Status Add(const AddRequest & request, AddResponse & response) { return libProtobufPipePlugin::Status::Factory::NotImplemented(__FUNCTION__); }
    private:
      std::vector<std::string> functions_;
    };

  }; //namespace CalculatorService

}; //namespace calculus

#endif //PROTOBUF_CALCULATOR_PIPE_H
