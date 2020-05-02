#include "TestPluginRun.h"
#include "testutils.h"
#include "protobuf_locator.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"
#include "rapidassist/environment.h"
#include "rapidassist/testing.h"

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
  ASSERT_TRUE( ra::filesystem::CreateDirectory(outdir.c_str()) );

  //protoc --plugin=protoc-gen-NAME=path/to/mybinary.exe --NAME_out=OUT_DIR
  //protoc --plugin=protoc-gen-foobar=[...]\src\Debug\protobuf-pipe-plugin.exe --foobar_out=[...]\src\Debug\output --proto_path=[...]\src\Debug\proto_files [...]\src\Debug\proto_files\addressbookservice.proto

  //build
  std::string cmdline;
  cmdline.append("protoc.exe --plugin=protoc-gen-");
  cmdline.append(getPluginShortName());
  cmdline.append("=");
  cmdline.append(getPluginFilePath());
  cmdline.append(" --");
  cmdline.append(getPluginShortName());
  cmdline.append("_out=");
  cmdline.append(outdir);
  cmdline.append(" --proto_path=");
  cmdline.append(getTestProtoPath());
  cmdline.append(" ");
  cmdline.append(getTestProtoFilePath());

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
  ASSERT_TRUE( ra::filesystem::CreateDirectory(outdir.c_str()) );

  //protoc --NAME_out=%TEMP%
  //protoc.exe looks for protoc-gen-NAME.exe in %PATH%

  //add plugin dir to %PATH%
  addApplicationPath(ra::process::GetCurrentProcessDir().c_str());

  //duplicate the plugin file adding the expected "protoc-gen-" prefix to the filename
  static const std::string PREFIX = "protoc-gen-";
  const std::string exec_path = getPluginFilePath();
  const std::string exec_dir = ra::process::GetCurrentProcessDir() + separator;
  std::string output_path = exec_path; ra::strings::Replace(output_path, exec_dir, exec_dir + PREFIX);
  ASSERT_TRUE( ra::filesystem::CopyFile(exec_path, output_path) );

  std::string plugin_filename = getPluginFileName();
  std::string plugin_name_we = ra::filesystem::GetFilenameWithoutExtension(plugin_filename.c_str());

  //build
  std::string cmdline;
  cmdline.append("protoc.exe --");
  cmdline.append(plugin_name_we);
  cmdline.append("_out=");
  cmdline.append(outdir);
  cmdline.append(" --proto_path=");
  cmdline.append(getTestProtoPath());
  cmdline.append(" ");
  cmdline.append(getTestProtoFilePath());
  
  //run the command
  printf("%s\n", cmdline.c_str());
  int returnCode = system(cmdline.c_str());
#ifdef __linux__
  returnCode = WEXITSTATUS(returnCode);
#endif
  ASSERT_EQ(0, returnCode) << "The command line '" << cmdline.c_str() << "' returned " << returnCode;
}
