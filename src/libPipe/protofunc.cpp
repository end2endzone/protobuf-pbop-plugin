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

#include "protofunc.h"

#include <google/protobuf/descriptor.pb.h>

#include "rapidassist/strings.h"

std::string ToCppNamespace(const std::string & iPackage)
{
  std::string output = iPackage;

  ra::strings::Replace(output, ".", "::");

  return output;
}

const google::protobuf::FileDescriptor* AddFileDescriptorToPool(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptor & iFile, bool iIncludeFile)
{
  int count = iFile.dependency_count();
  for(int i=0; i<count; i++)
  {
    const google::protobuf::FileDescriptor * dependency = iFile.dependency(i);
    AddFileDescriptorToPool(pool, *dependency, true);
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

const google::protobuf::FileDescriptor * BuildFileDescriptor(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptorProto & iFileProto)
{
  const google::protobuf::FileDescriptor* desc = pool.BuildFile(iFileProto);
  return desc;
}

const void ToFileDescriptorProto( const google::protobuf::FileDescriptor & iFile, google::protobuf::FileDescriptorProto & oFileProto)
{
  iFile.CopyTo(&oFileProto);
}

std::string ToProtoString(const google::protobuf::FileDescriptor & iFile)
{
  std::string output = iFile.DebugString();
  return output;
}

std::string ToProtoString(const google::protobuf::FileDescriptorProto & iFileProto)
{
  google::protobuf::DescriptorPool pool;
  pool.AllowUnknownDependencies();
  const google::protobuf::FileDescriptor* desc = BuildFileDescriptor(pool, iFileProto);
  if (!desc)
    return "";
  std::string output = ToProtoString(*desc);
  return output;
}
