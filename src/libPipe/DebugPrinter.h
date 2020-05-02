#pragma once

#include <google/protobuf/compiler/code_generator.h>

using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::io;

class DebugPrinter
{
public:
  DebugPrinter(GeneratorContext * generator_context);
  virtual ~DebugPrinter();

  void printFile(const FileDescriptor * file, const char * iFilename);

private:
  GeneratorContext * mGenerator;
};
