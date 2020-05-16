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

#include "libpipe.h"

//From RapidAssist, https://github.com/end2endzone/RapidAssist/blob/0.8.1/src/rapidassist/strings.cpp#L261
int Replace(std::string & iString, const std::string & iOldValue, const std::string & iNewValue) {
  int num_occurance = 0;

  if (iOldValue.size() > 0) {
    size_t start_pos = 0;
    size_t find_pos = std::string::npos;
    do {
      find_pos = iString.find(iOldValue, start_pos);
      if (find_pos != std::string::npos) {
        iString.replace(find_pos, iOldValue.length(), iNewValue);
        start_pos = find_pos + iNewValue.length();
        num_occurance++;
      }
    } while (find_pos != -1);
  }
  return num_occurance;
}

//From RapidAssist, https://github.com/end2endzone/RapidAssist/blob/0.8.1/src/rapidassist/filesystem.cpp#L586
void SplitPath(const std::string & iPath, std::string & oDirectory, std::string & oFilename) {
  oDirectory = "";
  oFilename = "";

  std::size_t offset = iPath.find_last_of("/\\");
  if (offset != std::string::npos) {
    //found
    oDirectory = iPath.substr(0, offset);
    oFilename = iPath.substr(offset + 1);
  }
  else {
    oFilename = iPath;
  }
}

//From RapidAssist, https://github.com/end2endzone/RapidAssist/blob/0.8.1/src/rapidassist/filesystem.cpp#L121
std::string GetFilename(const char * iPath) {
  if (iPath == NULL || iPath[0] == '\0')
    return "";

  std::string directory;
  std::string filename;
  SplitPath(iPath, directory, filename);

  return filename;
}

//From RapidAssist, https://github.com/end2endzone/RapidAssist/blob/0.8.1/src/rapidassist/filesystem.cpp#L684
std::string GetFileExtention(const std::string & iPath) {
  //extract filename from path to prevent
  //reading a directory's extension
  std::string directory;
  std::string filename;
  SplitPath(iPath, directory, filename);

  std::string extension;
  std::size_t offset = filename.find_last_of(".");
  if (offset != std::string::npos) {
    //found
    //name = filename.substr(0,offset);
    extension = filename.substr(offset + 1);
  }

  return extension;
}

//From RapidAssist, https://github.com/end2endzone/RapidAssist/blob/0.8.1/src/rapidassist/strings.cpp#L545
std::string TrimRight(const std::string & iStr, const char iChar) {
  static const size_t SIZE_T_MAX = std::numeric_limits< size_t >::max();

  std::string tmp = iStr;

  if (iChar == '\0')
    return tmp;

  size_t size = iStr.size();
  if (size) {
    size_t loop_start = size - 1;
    size_t loop_end = 0;
    for (size_t i = loop_start; i >= loop_end && i != SIZE_T_MAX; i--) {
      if (tmp[i] == iChar) {
        tmp.erase(i, 1);
      }
      else {
        //no need to continue looking
        break;
      }
    }
  }
  return tmp;
}

//From RapidAssist, https://github.com/end2endzone/RapidAssist/blob/0.8.1/src/rapidassist/filesystem.cpp#L132
std::string GetFilenameWithoutExtension(const char * iPath) {
  if (iPath == NULL || iPath[0] == '\0')
    return "";

  std::string filename = GetFilename(iPath);
  std::string extension = GetFileExtention(iPath);

  //extract filename without extension
  std::string filename_without_extension = filename.substr(0, filename.size() - extension.size());

  //remove last dot of the filename if required
  filename_without_extension = TrimRight(filename_without_extension, '.');

  return filename_without_extension;
}

std::string Uppercase(const std::string & iValue) {
  std::string copy = iValue;
  for (size_t i = 0; i < copy.size(); i++) {
    copy[i] = (char)toupper(copy[i]);
  }
  return copy;
}

std::string ToCppNamespace(const std::string & iPackage)
{
  std::string output = iPackage;

  Replace(output, ".", "::");

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
