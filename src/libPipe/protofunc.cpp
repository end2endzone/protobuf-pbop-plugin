#include "protofunc.h"
#include "stringfunc.h"

#include <google/protobuf/descriptor.pb.h>

std::string toCppNamespace(const std::string & iPackage)
{
  std::string output = iPackage;

  stringReplace(output, ".", "::");

  return output;
}

const google::protobuf::FileDescriptor* addFileDescriptorToPool(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptor & iFile, bool iIncludeFile)
{
  int count = iFile.dependency_count();
  for(int i=0; i<count; i++)
  {
    const google::protobuf::FileDescriptor * dependency = iFile.dependency(i);
    addFileDescriptorToPool(pool, *dependency, true);
  }

  //the file itself
  if (iIncludeFile)
  {
    google::protobuf::FileDescriptorProto tmpProto;
    iFile.CopyTo(&tmpProto);
    const google::protobuf::FileDescriptor* desc = pool.BuildFile(tmpProto);
    return desc;
  }

  return NULL;
}

const google::protobuf::FileDescriptor * buildFileDescriptor(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptorProto & iFileProto)
{
  const google::protobuf::FileDescriptor* desc = pool.BuildFile(iFileProto);
  return desc;
}

const void toFileDescriptorProto( const google::protobuf::FileDescriptor & iFile, google::protobuf::FileDescriptorProto & oFileProto)
{
  iFile.CopyTo(&oFileProto);
}

std::string toProtoString(const google::protobuf::FileDescriptor & iFile)
{
  std::string output = iFile.DebugString();
  return output;
}

std::string toProtoString(const google::protobuf::FileDescriptorProto & iFileProto)
{
  google::protobuf::DescriptorPool pool;
  pool.AllowUnknownDependencies();
  const google::protobuf::FileDescriptor* desc = buildFileDescriptor(pool, iFileProto);
  if (!desc)
    return "";
  std::string output = toProtoString(*desc);
  return output;
}
