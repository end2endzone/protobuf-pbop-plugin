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

#pragma once

#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/zero_copy_stream.h>

class PluginCodeGenerator : public google::protobuf::compiler::CodeGenerator
{
public:
  PluginCodeGenerator();
  virtual ~PluginCodeGenerator();

  virtual bool Generate(const google::protobuf::FileDescriptor * file, const std::string & parameter, google::protobuf::compiler::GeneratorContext * generator_context, std::string * error) const;
private:
  bool GenerateHeader(const google::protobuf::FileDescriptor * file, const std::string & parameter, google::protobuf::compiler::GeneratorContext * generator_context, std::string * error) const;
  bool GenerateSource(const google::protobuf::FileDescriptor * file, const std::string & parameter, google::protobuf::compiler::GeneratorContext * generator_context, std::string * error) const;
};
