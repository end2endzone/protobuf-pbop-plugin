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

#include "greetings.pb.h"
#include "greetings.pipe.pb.h"
#include "pbop/Server.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

static const char * kPipeName = "\\\\.\\pipe\\greetingspipe";

class GreeterServiceImpl : public greetings::Greeter::ServerStub
{
public:
  GreeterServiceImpl() {}
  virtual ~GreeterServiceImpl() {}

  pbop::Status SayHello(const greetings::SayHelloRequest & request, greetings::SayHelloResponse & response)
  {
    response.set_message("Greetings " + request.name());
    return pbop::Status::OK;
  }

  pbop::Status SayGoodbye(const greetings::SayGoodbyeRequest & request, greetings::SayGoodbyeResponse & response)
  {
    response.set_message("Farewell " + request.name());
    return pbop::Status::OK;
  }
};

std::string GetFilename(const std::string & path)
{
  size_t last_index = path.find_last_of("/\\");
  if (last_index == std::string::npos)
    last_index = 0;
  else
    last_index++;
  std::string filename = path.substr(last_index);
  return filename;
}

int main(int argc, char* argv[])
{
  printf("Running %s...\n", GetFilename(argv[0]).c_str());

  GreeterServiceImpl * impl = new GreeterServiceImpl();

  pbop::Server server;
  server.RegisterService(impl);
  pbop::Status status = server.Run(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }
  return 0;
}
