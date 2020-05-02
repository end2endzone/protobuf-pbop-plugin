#pragma once

#include <google/protobuf/descriptor.h>

#include <string>

std::string toCppNamespace(const std::string & iPackage);

///<summary>Add the given FileDescriptor (including its dependencies) to the given DescriptorPool.</summary>
///<param name="pool">The pool of files into which to add the given FileDescriptor (iFile).</param>
///<param name="iFile">The file descriptor to add to the pool.</param>
///<param name="iIncludeFile">True if you want to add the given . False if you only need its dependencies</param>
///<return>A FileDescriptor * which describes the given FileDescriptor (iFile).</return>
const google::protobuf::FileDescriptor * addFileDescriptorToPool(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptor & iFile, bool iIncludeFile);

const google::protobuf::FileDescriptor * buildFileDescriptor(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptorProto & iFileProto);

const void toFileDescriptorProto( const google::protobuf::FileDescriptor & iFile, google::protobuf::FileDescriptorProto & oFileProto);

std::string toProtoString(const google::protobuf::FileDescriptor & iFile);
std::string toProtoString(const google::protobuf::FileDescriptorProto & iFileProto);
