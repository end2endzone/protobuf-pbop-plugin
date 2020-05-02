#include "TestPath.h"
#include "path.h"

void TestPath::SetUp()
{
}

void TestPath::TearDown()
{
}

TEST_F(TestPath, testGetFileName)
{
  std::string path = "c:\\foo\\bar\\ding.txt";
  std::string filename = getFileName(path);

  ASSERT_EQ(std::string("ding.txt"), filename);
}

TEST_F(TestPath, testGetFileNameWithoutExtension)
{
  std::string path = "c:\\foo\\bar\\ding.txt";
  std::string filename = getFileNameWithoutExtension(path);

  ASSERT_EQ(std::string("ding"), filename);
}

TEST_F(TestPath, testGetFileExtension)
{
  std::string path = "c:\\foo\\bar\\ding.txt";
  std::string ext = getFileExtension(path);

  ASSERT_EQ(std::string(".txt"), ext);
}

TEST_F(TestPath, testRemoveFileExtension)
{
  std::string path = "c:\\foo\\bar\\ding.txt";
  removeFileExtension(path);

  ASSERT_EQ(std::string("c:\\foo\\bar\\ding"), path);
}

TEST_F(TestPath, testGetFolder)
{
  std::string path = "c:\\foo\\bar\\ding.txt";
  std::string folder = getFolder(path);
  ASSERT_EQ(std::string("c:\\foo\\bar"), folder);

  path = "c:\\foo\\ding.txt";
  folder = getFolder(path);
  ASSERT_EQ(std::string("c:\\foo"), folder);

  path = "c:\\ding.txt";
  folder = getFolder(path);
  ASSERT_EQ(std::string("c:"), folder);

  path = "ding.txt";
  folder = getFolder(path);
  ASSERT_EQ(std::string(""), folder);
}
