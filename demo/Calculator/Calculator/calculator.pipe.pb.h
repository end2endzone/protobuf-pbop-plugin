#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "calculator.pb.h"

#include "libProtobufPipePlugin/Status.h"
#include "libProtobufPipePlugin/Service.h"
#include "libProtobufPipePlugin/Connection.h"

#include <string>

namespace calculus
{

  namespace CalculatorService
  {
    class Service : public libProtobufPipePlugin::Service
    {
      //libProtobufPipePlugin::Service definition
      virtual const std::string & GetPackageName() const = 0;
      virtual const std::string & GetServiceName() const = 0;
      virtual const std::vector<std::string> & GetFunctionIdentifiers() const = 0;
      virtual libProtobufPipePlugin::Status DispatchMessage(const size_t & index, const std::string & input, std::string & output) = 0;

      //CalculatorService service implementation
      virtual libProtobufPipePlugin::Status Add(const AddRequest & request, AddResponse & response) = 0;
    };

    class Client : public Service
    {
    public:
      Client(libProtobufPipePlugin::Connection * connection);
      virtual ~Client();

      //libProtobufPipePlugin::Service definition
      virtual const std::string & GetPackageName() const;
      virtual const std::string & GetServiceName() const;
      virtual const std::vector<std::string> & GetFunctionIdentifiers() const;
      virtual libProtobufPipePlugin::Status DispatchMessage(const size_t & index, const std::string & input, std::string & output);

      //CalculatorService service implementation
      virtual libProtobufPipePlugin::Status Add(const AddRequest & request, AddResponse & response);

    private:
      libProtobufPipePlugin::Connection * connection_;
    };

    class ServerStub : public Service
    {
    public:
      ServerStub();
      virtual ~ServerStub();

      //libProtobufPipePlugin::Service definition
      virtual const std::string & GetPackageName() const;
      virtual const std::string & GetServiceName() const;
      virtual const std::vector<std::string> & GetFunctionIdentifiers() const;
      virtual libProtobufPipePlugin::Status DispatchMessage(const size_t & index, const std::string & input, std::string & output);

      //CalculatorService service implementation
      virtual libProtobufPipePlugin::Status Add(const AddRequest & request, AddResponse & response);
    private:
      std::vector<std::string> functions_;
    };

  }; //namespace CalculatorService

}; //namespace calculus

#endif //TEMPLATE_H
