#include "PluginCodeGenerator.h"
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include "StreamPrinter.h"
#include "DebugPrinter.h"
#include "stringfunc.h"
#include "protofunc.h"

#include <windows.h>

PluginCodeGenerator::PluginCodeGenerator()
{
#if 0
  static const char * caption = "Debug breakpoint";
  std::string message = "protobuf-dcom-plugin constructor.";
  MessageBox(NULL,message.c_str(), caption, 0);
  int a = 0;
#endif
}

PluginCodeGenerator::~PluginCodeGenerator()
{
}

bool PluginCodeGenerator::Generate(const google::protobuf::FileDescriptor * file, const std::string & parameter, google::protobuf::compiler::GeneratorContext * generator_context, string * error) const
{
#if 0
  static const char * caption = "Debug breakpoint";
  std::string message = "protobuf-dcom-plugin" + std::string("\n\nFile: '") + file->name() + "'.";
  MessageBox(NULL,message.c_str(), caption, 0);
  int a = 0;
#endif

  //Show content of the file
  DebugPrinter debugger(generator_context);
  debugger.printFile(file, "debug.txt");

  if (error)
  {
    std::string & e = (*error);
    //e = "unknown error";
  }

  return true;
}
