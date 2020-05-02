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

#include <string>

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
