#include <stdio.h>

#ifdef _WIN32
//google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
//google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)
__pragma( warning(push) )
__pragma( warning(disable: 4800))
__pragma( warning(disable: 4146))
#endif //_WIN32

#include "calculator.pipe.pb.h"
#include "libProtobufPipePlugin/Server.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

using namespace libProtobufPipePlugin;
using namespace calculus;

static const char * kPipeName = "\\\\.\\pipe\\mynamedpipe";

class CalculatorServiceImpl : public CalculatorService::ServerStub
{
public:
  CalculatorServiceImpl() {}
  virtual ~CalculatorServiceImpl() {}

  Status Add(const AddRequest & request, AddResponse & response)
  {
    const int & left = request.left();
    const int & right = request.right();
    int result = left + right;
    response.set_value(result);
    return Status::OK;
  }

};

int RunClient()
{
  printf("Launching client...\n");

  PipeConnection * connection = new PipeConnection();
  Status status = connection->Connect(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  CalculatorService::Client client(connection);

  AddRequest request;
  AddResponse response;

  int left = 15;
  int right = 3;

  request.set_left(left);
  request.set_right(right);

  printf("Calling Add(%d,%d)\n", left, right);
  status = client.Add(request, response);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  printf("The result is %d\n", response.value());

  return 0;
}

int RunServer()
{
  printf("Launching server...\n");

  CalculatorServiceImpl * impl = new CalculatorServiceImpl();

  Server server;
  server.RegisterService(impl);
  Status status = server.Run(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }
  return 0;
}

int main(int argc, char* argv[])
{
  if (argc == 2 && std::string(argv[1]) == "client")
  {
    return RunClient();
  }
  else if (argc == 2 && std::string(argv[1]) == "server")
  {
    return RunServer();
  }
  else
  {
    printf("Error. Launch program with 'client' or 'server' argument\n");
    return 1;
  }
}
