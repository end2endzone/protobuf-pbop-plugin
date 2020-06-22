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

#include "TestPluginRun.h"

#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"
#include "rapidassist/environment.h"
#include "rapidassist/testing.h"

#include "TestUtils.h"
#include "protobuf_locator.h"

void TestPluginRun::SetUp()
{
}

void TestPluginRun::TearDown()
{
}

TEST_F(TestPluginRun, testShowProtocVersion)
{
  std::string cmdline;
  cmdline.append("protoc.exe --version");

  //run the command
  int returnCode = system(cmdline.c_str());
#ifdef __linux__
  returnCode = WEXITSTATUS(returnCode);
#endif
  ASSERT_EQ(0, returnCode) << "The command line '" << cmdline.c_str() << "' returned " << returnCode;
}

TEST_F(TestPluginRun, testRunPluginAbsolutePath)
{
  //create output dir
  static const std::string separator = ra::filesystem::GetPathSeparatorStr();
  std::string outdir = ra::process::GetCurrentProcessDir() + separator + ra::testing::GetTestQualifiedName();
  if (ra::filesystem::DirectoryExists(outdir.c_str()))
    ASSERT_TRUE( ra::filesystem::DeleteDirectory(outdir.c_str()) );
#if _WIN32
  //https://stackoverflow.com/questions/32257708/directory-createdirectory-does-not-always-create-the-folder
  //https://stackoverflow.com/questions/32593552/using-directory-delete-and-directory-createdirectory-to-overwrite-a-folder
  while(ra::filesystem::DirectoryExists(outdir.c_str())) {}
#endif
  ASSERT_TRUE( ra::filesystem::CreateDirectory(outdir.c_str()) );
  ASSERT_TRUE( ra::filesystem::DirectoryExists(outdir.c_str()) );

  //protoc --plugin=protoc-gen-NAME=path/to/mybinary.exe --NAME_out=OUT_DIR
  //protoc --plugin=protoc-gen-foobar=[...]\src\Debug\protobuf-pbop-plugin.exe --foobar_out=[...]\src\Debug\output --proto_path=[...]\src\Debug\proto_files [...]\src\Debug\proto_files\addressbookservice.proto

  //build
  std::string cmdline;
  cmdline.append("protoc.exe --plugin=protoc-gen-");
  cmdline.append(GetPluginShortName());
  cmdline.append("=");
  cmdline.append(GetPluginFilePath());
  cmdline.append(" --");
  cmdline.append(GetPluginShortName());
  cmdline.append("_out=");
  cmdline.append(outdir);
  cmdline.append(" --proto_path=");
  cmdline.append(GetTestProtoPath());
  cmdline.append(" ");
  cmdline.append(GetTestProtoFilePath());

  //run the command
  printf("%s\n", cmdline.c_str());
  int returnCode = system(cmdline.c_str());
#ifdef __linux__
  returnCode = WEXITSTATUS(returnCode);
#endif
  ASSERT_EQ(0, returnCode) << "The command line '" << cmdline.c_str() << "' returned " << returnCode;
}

TEST_F(TestPluginRun, testRunPluginAutoDetect)
{
  //create output dir
  static const std::string separator = ra::filesystem::GetPathSeparatorStr();
  std::string outdir = ra::process::GetCurrentProcessDir() + separator + ra::testing::GetTestQualifiedName();
  if (ra::filesystem::DirectoryExists(outdir.c_str()))
    ASSERT_TRUE( ra::filesystem::DeleteDirectory(outdir.c_str()) );
#if _WIN32
  //https://stackoverflow.com/questions/32257708/directory-createdirectory-does-not-always-create-the-folder
  //https://stackoverflow.com/questions/32593552/using-directory-delete-and-directory-createdirectory-to-overwrite-a-folder
  while(ra::filesystem::DirectoryExists(outdir.c_str())) {}
#endif
  ASSERT_TRUE( ra::filesystem::CreateDirectory(outdir.c_str()) );
  ASSERT_TRUE( ra::filesystem::DirectoryExists(outdir.c_str()) );

  //protoc --NAME_out=%TEMP%
  //protoc.exe looks for protoc-gen-NAME.exe in %PATH%

  //add plugin dir to %PATH%
  AddApplicationPath(ra::process::GetCurrentProcessDir().c_str());

  //duplicate the plugin file adding the expected "protoc-gen-" prefix to the filename
  static const std::string PREFIX = "protoc-gen-";
  const std::string exec_path = GetPluginFilePath();
  const std::string exec_dir = ra::process::GetCurrentProcessDir() + separator;
  std::string output_path = exec_path; ra::strings::Replace(output_path, exec_dir, exec_dir + PREFIX);
  ASSERT_TRUE( ra::filesystem::CopyFile(exec_path, output_path) );

  std::string plugin_filename = GetPluginFileName();
  std::string plugin_name_we = ra::filesystem::GetFilenameWithoutExtension(plugin_filename.c_str());

  //build
  std::string cmdline;
  cmdline.append("protoc.exe --");
  cmdline.append(plugin_name_we);
  cmdline.append("_out=");
  cmdline.append(outdir);
  cmdline.append(" --proto_path=");
  cmdline.append(GetTestProtoPath());
  cmdline.append(" ");
  cmdline.append(GetTestProtoFilePath());
  
  //run the command
  printf("%s\n", cmdline.c_str());
  int returnCode = system(cmdline.c_str());
#ifdef __linux__
  returnCode = WEXITSTATUS(returnCode);
#endif
  ASSERT_EQ(0, returnCode) << "The command line '" << cmdline.c_str() << "' returned " << returnCode;
}
