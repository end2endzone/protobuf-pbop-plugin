#include "TestPluginRun.h"
#include "testutils.h"
#include "protobuf_locator.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"

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
  std::string outdir = getTestOutDir();
  ra::filesystem::CreateDirectory(outdir.c_str());

  //delete all files in output dir
  std::vector<std::string> outdir_files;
  ra::filesystem::FindFiles(outdir_files, outdir.c_str());
  for(size_t i=0; i<outdir_files.size(); i++)
  {
    const std::string & filePath = outdir_files[i];
    remove(filePath.c_str());
  }

  //protoc --plugin=protoc-gen-NAME=path/to/mybinary.exe --NAME_out=OUT_DIR
  //protoc --plugin=protoc-gen-foobar=[...]\src\Debug\protobuf-pipe-plugin.exe --foobar_out=[...]\src\Debug\output --proto_path=[...]\src\Debug\proto_files [...]\src\Debug\proto_files\addressbookservice.proto

  //build
  std::string cmdline;
  cmdline.append("protoc.exe --plugin=protoc-gen-");
  cmdline.append(getPluginName());
  cmdline.append("=");
  cmdline.append(getPluginFilePath());
  cmdline.append(" --");
  cmdline.append(getPluginName());
  cmdline.append("_out=");
  cmdline.append(getTestOutDir());
  cmdline.append(" --proto_path=");
  cmdline.append(getTestProtoPath());
  cmdline.append(" ");
  cmdline.append(getTestProtoFilePath());

  //run the command
  int returnCode = system(cmdline.c_str());
#ifdef __linux__
  returnCode = WEXITSTATUS(returnCode);
#endif
  ASSERT_EQ(0, returnCode) << "The command line '" << cmdline.c_str() << "' returned " << returnCode;
}

TEST_F(TestPluginRun, testRunPluginAutoDetect)
{
  //create output dir
  std::string outdir = getTestOutDir();
  ra::filesystem::CreateDirectory(outdir.c_str());

  //delete all files in output dir
  std::vector<std::string> outdir_files;
  ra::filesystem::FindFiles(outdir_files, outdir.c_str());
  for(size_t i=0; i<outdir_files.size(); i++)
  {
    const std::string & filePath = outdir_files[i];
    remove(filePath.c_str());
  }

  //protoc --NAME_out=%TEMP%
  //protoc.exe looks for protoc-gen-NAME.exe in %PATH%


  //add plugin dir to %PATH%
  addApplicationPath(ra::process::GetCurrentProcessDir().c_str());

  //duplicate the plugin file adding the expected "protoc-gen-" prefix to the filename
  std::string cmdline;
  cmdline = "";
  cmdline.append("copy \"");
  cmdline.append(getPluginFilePath());
  cmdline.append("\" \"");
  cmdline.append(ra::process::GetCurrentProcessDir());
  cmdline.append("\\protoc-gen-");
  cmdline.append(getPluginFileName());
  cmdline.append("\" >NUL 2>NUL");

  //run the command
  int returnCode = system(cmdline.c_str());
#ifdef __linux__
  returnCode = WEXITSTATUS(returnCode);
#endif
  ASSERT_EQ(0, returnCode) << "The command line '" << cmdline.c_str() << "' returned " << returnCode;

  std::string plugin_filename = getPluginFileName();
  std::string plugin_extension = ra::filesystem::GetFileExtention(plugin_filename);
  std::string plugin_name = plugin_filename; ra::strings::Replace(plugin_name, plugin_extension, "");

  //build
  cmdline = "";
  cmdline.append("protoc.exe --");
  cmdline.append(plugin_name);
  cmdline.append("_out=");
  cmdline.append(getTestOutDir());
  cmdline.append(" --proto_path=");
  cmdline.append(getTestProtoPath());
  cmdline.append(" ");
  cmdline.append(getTestProtoFilePath());
  
  //run the command
  returnCode = system(cmdline.c_str());
#ifdef __linux__
  returnCode = WEXITSTATUS(returnCode);
#endif
  ASSERT_EQ(0, returnCode) << "The command line '" << cmdline.c_str() << "' returned " << returnCode;
}
