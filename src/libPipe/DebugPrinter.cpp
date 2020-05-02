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
  info.print("Found package: %s" NEWLINE, package.c_str());

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
      info.print("Found UnknownField (FIXED32): %d" NEWLINE, field.fixed32());
      break;
    case google::protobuf::UnknownField::TYPE_FIXED64:
      info.print("Found UnknownField (FIXED64): %d" NEWLINE, field.fixed64());
      break;
    case google::protobuf::UnknownField::TYPE_GROUP:
      info.print("Found UnknownField (GROUP): ..." NEWLINE);
      break;
    case google::protobuf::UnknownField::TYPE_LENGTH_DELIMITED:
      info.print("Found UnknownField (LENGTH_DELIMITED): %s" NEWLINE, field.length_delimited().c_str());
      break;
    case google::protobuf::UnknownField::TYPE_VARINT:
      info.print("Found UnknownField (VARINT): %d" NEWLINE, field.varint());
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
    info.print("Found dependency: %s" NEWLINE, name.c_str());
  }

  //enums
  int numEnumTypes = file->enum_type_count();
  for(int i=0; i<numEnumTypes; i++)
  {
    const google::protobuf::EnumDescriptor * d = file->enum_type(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.print("Found enum: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
  }

  //extensions
  const google::protobuf::FieldDescriptor * my_file_option = NULL;
  int numExtensions = file->extension_count();
  for(int i=0; i<numExtensions; i++)
  {
    const google::protobuf::FieldDescriptor * d = file->extension(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.print("Found extension: %s (%s) typename=%s" NEWLINE, name.c_str(), fullname.c_str(), d->type_name());

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
    info.print("Found message: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
  }

  //public dependencies
  int numPublicDependencies = file->public_dependency_count();
  for(int i=0; i<numPublicDependencies; i++)
  {
    const google::protobuf::Descriptor * d = file->message_type(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.print("Found public dependency: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
  }

  //services
  int numServices = file->service_count();
  for(int i=0; i<numServices; i++)
  {
    const google::protobuf::ServiceDescriptor * d = file->service(i);
    const std::string fullname = d->full_name();
    const std::string & name = d->name();
    info.print("Found service: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
    
    int numMethods = d->method_count();
    for(int j=0; j<numMethods; j++)
    {
      const google::protobuf::MethodDescriptor * m = d->method(j);
      {
        const std::string fullname = m->full_name();
        const std::string & name = m->name();
        info.print("  found method: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
      }

      const google::protobuf::Descriptor * inputDesc = m->input_type();
      {
        const std::string fullname = inputDesc->full_name();
        const std::string & name = inputDesc->name();
        info.print("  input:  %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
      }

      const google::protobuf::Descriptor * outputDesc = m->output_type();
      {
        const std::string fullname = outputDesc->full_name();
        const std::string & name = outputDesc->name();
        info.print("  output: %s (%s)" NEWLINE, name.c_str(), fullname.c_str());
      }
    }
  }

  //weak dependencies
  int numWeakDependencies = file->weak_dependency_count();
  for(int i=0; i<numWeakDependencies; i++)
  {
    const google::protobuf::FileDescriptor * d = file->weak_dependency(i);
    const std::string & name = d->name();
    info.print("Found weak dependency: %s" NEWLINE, name.c_str());
  }

}
