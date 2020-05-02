#pragma once

#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/zero_copy_stream.h>

//using namespace google::protobuf;
//using namespace google::protobuf::compiler;
//using namespace google::protobuf::io;

class PluginCodeGenerator : public google::protobuf::compiler::CodeGenerator
{
public:
  PluginCodeGenerator();
  virtual ~PluginCodeGenerator();

  virtual bool Generate(const google::protobuf::FileDescriptor * file, const std::string & parameter, google::protobuf::compiler::GeneratorContext * generator_context, std::string * error) const;
};
