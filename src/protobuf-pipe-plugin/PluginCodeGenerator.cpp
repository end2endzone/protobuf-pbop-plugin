#include "PluginCodeGenerator.h"
#include <google/protobuf/compiler/cpp/cpp_generator.h>

#include "rapidassist/strings.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/environment.h"
#include "rapidassist/process.h"

#include "StreamPrinter.h"
#include "DebugPrinter.h"
#include "protofunc.h"

//for debugging
#include <Windows.h>

PluginCodeGenerator::PluginCodeGenerator()
{
#if 0
  const std::string exe_path = ra::process::GetCurrentProcessPath();
  const std::string exe_filename = ra::filesystem::GetFilename(exe_path.c_str());
  std::string message = __FUNCTION__;
  MessageBoxA(NULL,message.c_str(), exe_filename.c_str(), MB_OK + MB_ICONEXCLAMATION);
  int a = 0;
#endif
}

PluginCodeGenerator::~PluginCodeGenerator()
{
}

bool PluginCodeGenerator::Generate(const google::protobuf::FileDescriptor * file, const std::string & parameter, google::protobuf::compiler::GeneratorContext * generator_context, string * error) const
{
#if 0
  const std::string exe_path = ra::process::GetCurrentProcessPath();
  const std::string exe_filename = ra::filesystem::GetFilename(exe_path.c_str());
  std::string message = __FUNCTION__;
  MessageBoxA(NULL,message.c_str(), exe_filename.c_str(), MB_OK + MB_ICONEXCLAMATION);
  int a = 0;
#endif

  //Show content of the file
  DebugPrinter debugger(generator_context);
  debugger.PrintFile(file, "debug.txt");

  std::string header_filename = ra::filesystem::GetFilenameWithoutExtension(file->name().c_str()) << ".pipe.pb.h";

  //printf a stub header
  {
    google::protobuf::io::ZeroCopyOutputStream * stream = generator_context->Open(header_filename.c_str());
    StreamPrinter printer(stream); //StreamPrinter takes ownership of the Stream
    printer.print("//hello world in header!\n");
  }

  //printf a stub cpp
  {
    std::string cpp_filename = ra::filesystem::GetFilenameWithoutExtension(file->name().c_str()) << ".pipe.pb.cpp";
    google::protobuf::io::ZeroCopyOutputStream * stream = generator_context->Open(cpp_filename.c_str());
    StreamPrinter printer(stream); //StreamPrinter takes ownership of the Stream
    printer.print("#include \"%s\"\n", header_filename.c_str());
  }

  if (error)
  {
    std::string & e = (*error);
    //e = "unknown error";
  }

  return true;
}
