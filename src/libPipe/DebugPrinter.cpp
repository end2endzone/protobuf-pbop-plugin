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

#include "DebugPrinter.h"
#include "StreamPrinter.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream.h>

#ifdef _WIN32
#  define NEWLINE "\r\n"
#else
#  define NEWLINE "\n"
#endif

DebugPrinter::DebugPrinter(GeneratorContext * generator_context) :
  mGenerator(generator_context)
{
}

DebugPrinter::~DebugPrinter()
{
}

void DebugPrinter::PrintFile(const FileDescriptor * file, const char * iFilename)
{
  google::protobuf::io::ZeroCopyOutputStream * infoStream = mGenerator->Open(iFilename);
  StreamPrinter info(infoStream); //StreamPrinter takes ownership of the Stream

  //package
  const std::string & package = file->package();
  info.Print("Found package: %s" NEWLINE, package.c_str());

  //options
  const google::protobuf::FileOptions & options = file->options();
  const google::protobuf::UnknownFieldSet & unknownFieldSet = options.unknown_fields();
  int numUnknownFields = unknownFieldSet.field_count();
  for(int i=0; i<numUnknownFields; i++)
  {
    const google::protobuf::UnknownField & field = unknownFieldSet.field(i);
    google::protobuf::UnknownField::Type type = field.type();
    switch(type)
    {
    case google::protobuf::UnknownField::TYPE_FIXED32:
      info.Print("Found UnknownField (FIXED32): %d" NEWLINE, field.fixed32());
      break;
    case google::protobuf::UnknownField::TYPE_FIXED64:
      info.Print("Found UnknownField (FIXED64): %d" NEWLINE, field.fixed64());
      break;
    case google::protobuf::UnknownField::TYPE_GROUP:
      info.Print("Found UnknownField (GROUP): ..." NEWLINE);
      break;
    case google::protobuf::UnknownField::TYPE_LENGTH_DELIMITED:
      info.Print("Found UnknownField (LENGTH_DELIMITED): %s" NEWLINE, field.length_delimited().c_str());
      break;
    case google::protobuf::UnknownField::TYPE_VARINT:
      info.Print("Found UnknownField (VARINT): %d" NEWLINE, field.varint());
      break;
    //default:
    //  char errorMessage[1024];
    //  sprintf(errorMessage, "Unknown google::protobuf::UnknownField::Type %d", type);
    //  (*error) = errorMessage;
    //  return false;
    };
  }

  //dependencies
  int numDependencies = file->dependency_count();
  for(int i=0; i<numDependencies; i++)
  {
    const google::protobuf::FileDescriptor * d = file->dependency(i);
    const std::string & name = d->name();
    info.Print("Found dependency: %s" NEWLINE, name.c_str());
  }

  //enums
  int numEnumTypes = file->enum_type_count();
  for(int i=0; i<numEnumTypes; i++)
  {
    const google::protobuf::EnumDescriptor * d = file->enum_type(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.Print("Found enum: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
  }

  //extensions
  const google::protobuf::FieldDescriptor * my_file_option = NULL;
  int numExtensions = file->extension_count();
  for(int i=0; i<numExtensions; i++)
  {
    const google::protobuf::FieldDescriptor * d = file->extension(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.Print("Found extension: %s (%s) typename=%s" NEWLINE, name.c_str(), fullname.c_str(), d->type_name());

    const google::protobuf::Descriptor * desc = d->message_type();
    
    const google::protobuf::FieldDescriptor::CppType type = d->cpp_type();
  }
  
  ////search for custom options

  ////string value = MyMessage::descriptor()->options().GetExtension(my_option);
  ////string value = options.GetExtension(my_file_option);
  //if (my_file_option != NULL)
  //{
  //  //options.GetExtension(*my_file_option, 0);

  //}
  ////options.unkno

  //string value = options.GetExtension(customoptions::my_file_option);
  //info.print("Found custom option: value=%s\n", value.c_str());

  

  //messages
  int numMessageTypes = file->message_type_count();
  for(int i=0; i<numMessageTypes; i++)
  {
    const google::protobuf::Descriptor * d = file->message_type(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.Print("Found message: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
  }

  //public dependencies
  int numPublicDependencies = file->public_dependency_count();
  for(int i=0; i<numPublicDependencies; i++)
  {
    const google::protobuf::Descriptor * d = file->message_type(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.Print("Found public dependency: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
  }

  //services
  int numServices = file->service_count();
  for(int i=0; i<numServices; i++)
  {
    const google::protobuf::ServiceDescriptor * d = file->service(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.Print("Found service: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
    
    int numMethods = d->method_count();
    for(int j=0; j<numMethods; j++)
    {
      const google::protobuf::MethodDescriptor * m = d->method(j);
      {
        const std::string fullname = m->full_name();
        const std::string & name = m->name();
        info.Print("  found method: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
      }

      const google::protobuf::Descriptor * inputDesc = m->input_type();
      {
        const std::string fullname = inputDesc->full_name();
        const std::string & name = inputDesc->name();
        info.Print("  input:  %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
      }

      const google::protobuf::Descriptor * outputDesc = m->output_type();
      {
        const std::string fullname = outputDesc->full_name();
        const std::string & name = outputDesc->name();
        info.Print("  output: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
      }
    }
  }

  //weak dependencies
  int numWeakDependencies = file->weak_dependency_count();
  for(int i=0; i<numWeakDependencies; i++)
  {
    const google::protobuf::FileDescriptor * d = file->weak_dependency(i);
    const std::string & name = d->name();
    info.Print("Found weak dependency: %s" NEWLINE, name.c_str());
  }

}
