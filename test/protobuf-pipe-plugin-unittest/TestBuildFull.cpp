#include "TestBuildFull.h"
#include "testutils.h"
#include "rapidassist/filesystem.h"

void TestBuildFull::SetUp()
{
}

void TestBuildFull::TearDown()
{
}

TEST_F(TestBuildFull, testRun)
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

  //run command line
  ASSERT_EXEC(cmdline.c_str());
}
