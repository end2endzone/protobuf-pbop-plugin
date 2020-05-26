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

#ifndef LIB_PROTOBUF_PIPE_PLUGIN_FUNCTIONS
#define LIB_PROTOBUF_PIPE_PLUGIN_FUNCTIONS

#ifdef _WIN32
//google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
//google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)
__pragma( warning(push) )
__pragma( warning(disable: 4800))
__pragma( warning(disable: 4146))
#endif //_WIN32

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

#include <string>

namespace pbop
{

  int Replace(std::string & iString, const std::string & iOldValue, const std::string & iNewValue);
  std::string GetFilenameWithoutExtension(const char * iPath);
  std::string Uppercase(const std::string & iValue);

  std::string ToCppNamespace(const std::string & iPackage);

  ///<summary>Add the given FileDescriptor (including its dependencies) to the given DescriptorPool.</summary>
  ///<param name="pool">The pool of files into which to add the given FileDescriptor (iFile).</param>
  ///<param name="iFile">The file descriptor to add to the pool.</param>
  ///<param name="iIncludeFile">True if you want to add the given . False if you only need its dependencies</param>
  ///<return>A FileDescriptor * which describes the given FileDescriptor (iFile).</return>
  const google::protobuf::FileDescriptor * AddFileDescriptorToPool(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptor & iFile, bool iIncludeFile);

  const google::protobuf::FileDescriptor * BuildFileDescriptor(google::protobuf::DescriptorPool & pool, const google::protobuf::FileDescriptorProto & iFileProto);

  const void ToFileDescriptorProto( const google::protobuf::FileDescriptor & iFile, google::protobuf::FileDescriptorProto & oFileProto);

  std::string ToProtoString(const google::protobuf::FileDescriptor & iFile);
  std::string ToProtoString(const google::protobuf::FileDescriptorProto & iFileProto);

}; //namespace pbop

#endif //LIB_PROTOBUF_PIPE_PLUGIN_FUNCTIONS
