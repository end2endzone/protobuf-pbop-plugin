#include "TestDemo.h"
#include "testutils.h"

void TestDemo::SetUp()
{
}

void TestDemo::TearDown()
{
}

TEST_F(TestDemo, testRun)
{
  //create output dir
  std::string outdir = getTestOutDir();
  mkdir(outdir.c_str());

  //delete all files in output dir
  std::vector<std::string> outdir_files = get_all_files_names_within_folder(outdir);
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

TEST_F(TestDemo, testRunTwice)
{
  //create second output dir
  std::string outdir2 = getTestOutDir()+"2";
  mkdir(outdir2.c_str());

  //delete all files in output dir
  std::vector<std::string> outdir2Files = get_all_files_names_within_folder(outdir2);
  for(size_t i=0; i<outdir2Files.size(); i++)
  {
    const std::string & filePath = outdir2Files[i];
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
  cmdline.append(outdir2);
  cmdline.append(" --proto_path=");
  cmdline.append(getTestProtoPath());
  cmdline.append(" ");
  cmdline.append(getTestProtoFilePath());

  //run command line
  ASSERT_EXEC(cmdline.c_str());

  //make sure that generating the same file twice generates the same output
  ASSERT_TRUE( isFolderEquals(getTestOutDir(), outdir2) );
}
