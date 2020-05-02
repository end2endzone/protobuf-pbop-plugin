#include "testutils.h"
#include "rapidassist/testing.h"
#include "rapidassist/environment.h"
#include "rapidassist/filesystem.h"
#include "rapidassist/process.h"
#include "protobuf_locator.h"
#include <algorithm>

static const char * PROTOBUF_PIPE_PLUGIN_NAME = "protobuf-pipe-plugin";

std::string getPluginName()
{
  return PROTOBUF_PIPE_PLUGIN_NAME;
}

std::string getPluginFileName()
{
  std::string path = getPluginFilePath();
  std::string filename = ra::filesystem::GetFilename(path.c_str());
  return filename;
}

std::string getPluginFilePath()
{
  std::string path = ra::process::GetCurrentProcessPath();

  //replace test executable name by plugin executable name
  static const std::string plugin_name = PROTOBUF_PIPE_PLUGIN_NAME;
  static const std::string   test_name = "protobuf-pipe-plugin_unittest";
  ra::strings::Replace(path, test_name, plugin_name);

  return path;
}

std::string getTestOutDir()
{
  std::string outdir;
  outdir.append(ra::process::GetCurrentProcessDir());
  outdir.append("\\");
  outdir.append(ra::testing::GetTestSuiteName());
  outdir.append("Out");
  return outdir;
}

std::string getTestProtoFilePath()
{
  std::string outdir;
  outdir.append(ra::process::GetCurrentProcessDir());
  outdir.append("\\");
  outdir.append(ra::testing::GetTestSuiteName());
  outdir.append(".proto");
  return outdir;
}

std::string getTestProtoPath()
{
  std::string outdir;
  outdir.append(getProtobufIncludeDirectory());
  outdir.append(";");
  outdir.append(ra::process::GetCurrentProcessDir());
  outdir.append(";");
  outdir.append(getTestOutDir());
  return outdir;
}

void addApplicationPath(const char * iPath)
{
  std::string old_path = ra::environment::GetEnvironmentVariable("PATH");

  std::string new_path;
  new_path.append(iPath);
  new_path.append(";");
  new_path.append(old_path);

  ra::environment::SetEnvironmentVariable("PATH", new_path.c_str());
}

void eraseToken(std::string & iString, const std::string & iToken)
{
  size_t pos = iString.find(iToken);
  if (pos != std::string::npos)
  {
    //found
    iString.erase(iString.begin()+pos, iString.begin()+pos+iToken.size());
  }
}

bool hasMacroInFile(const char * iFilePath, std::string & oError)
{
  size_t fileSize = ra::filesystem::GetFileSize(iFilePath);

  FILE * f = fopen(iFilePath, "rb");
  if (!f)
  {
    std::string desc;
    desc = "File '" + std::string(iFilePath) + "' was not found!";
    throw std::exception(desc.c_str());
  }

  char * buffer = new char[fileSize];
  if (!buffer)
  {
    fclose(f);

    std::string desc;
    desc = "Not enough memory.";
    throw std::exception(desc.c_str());
  }

  size_t numBytesRead = fread(buffer, 1, fileSize, f);
  if (numBytesRead != fileSize)
  {
    fclose(f);
    delete[] buffer;

    std::string desc;
    desc = "read buffer too small.";
    throw std::exception(desc.c_str());
  }

  //file in buffer;
  fclose(f);

  bool macroFound = false;

  //search in buffer for COMGEN_ which is the old prefix for macros
  {
    static const std::string pattern = "COMGEN_";
    for(size_t i=0; i<fileSize && !macroFound; i++)
    {
      if (memcmp(buffer+i, pattern.c_str(), pattern.size()) == 0)
      {
        //search for the name of the macros
        //std::string content = buffer+i;
        //size_t pos = content.find(' ')

        std::string macroName = findWordName(buffer+i);
        oError = "File '" + std::string(iFilePath) + "' has macros left: " + macroName + ".";
        macroFound = true;
      }
    }
  }

  //search in buffer for DCOMPLUGIN_ macro prefix
  {
    static const std::string pattern = "DCOMPLUGIN_";
    for(size_t i=0; i<fileSize && !macroFound; i++)
    {
      if (memcmp(buffer+i, pattern.c_str(), pattern.size()) == 0)
      {
        //search for the name of the macros
        //std::string content = buffer+i;
        //size_t pos = content.find(' ')

        std::string macroName = findWordName(buffer+i);
        oError = "File '" + std::string(iFilePath) + "' has macros left: " + macroName + ".";
        macroFound = true;
      }
    }
  }

  delete[] buffer;
  return macroFound;
}

bool hasMacroInFiles(const std::vector<std::string> & iFilesPath, std::string & oError)
{
  for(size_t i=0; i<iFilesPath.size(); i++)
  {
    const std::string & path = iFilesPath[i];
    bool found = hasMacroInFile(path.c_str(), oError);
    if (found)
    {
      return true;
    }
  }

  oError = "";
  return false;
}

size_t findFirst(const std::string & iValue, const std::vector<char> & iTokens)
{
  size_t firstPos = std::string::npos;

  for(size_t i=0; i<iTokens.size(); i++)
  {
    char c = iTokens[i];
    size_t pos = iValue.find(c);
    if (pos < firstPos)
      firstPos = pos;
  }

  return firstPos;
}

std::string findWordName(const std::string & iBuffer)
{
  //idenfity characters that ends a word

  std::vector<char> tokens;
  tokens.push_back('\0'); //do not look further than the end of the string/file

  for(int i=1; i<256; i++)
  {
    char c = i;
    if (c == '_')
    {
      //ok
    }
    else if (c >= 'a' && c <= 'z')
    {
      //ok
    }
    else if (c >= 'A' && c <= 'Z')
    {
      //ok
    }
    else if (c >= '0' && c <= '9')
    {
      //ok
    }
    else
    {
      //end of word marker
      tokens.push_back(c);
    }
  }

  size_t pos = findFirst(iBuffer, tokens);
  if (pos == std::string::npos)
  {
    //no end of word was found
    return "";
  }

  std::string word = iBuffer.substr(0, pos);
  return word;
}

bool isFolderEquals(const std::string & folderA, const std::string & folderB)
{
  ra::strings::StringVector filesA;
  ra::strings::StringVector filesB;
  ra::filesystem::FindFiles(filesA, folderA.c_str());
  ra::filesystem::FindFiles(filesB, folderB.c_str());
  if (filesA.size() != filesB.size())
    return false;

  //remove the folder from each lists
  for(size_t i=0; i<filesA.size(); i++)
  {
    std::string & filePathA = filesA[i];
    ra::strings::Replace(filePathA, folderA + "\\", "");
    
    std::string & filePathB = filesB[i];
    ra::strings::Replace(filePathB, folderB + "\\", "");
  }

  //sort both folders
  std::sort(filesA.begin(), filesA.end());
  std::sort(filesB.begin(), filesB.end());

  //file names must be equals
  for(size_t i=0; i<filesA.size(); i++)
  {
    const std::string & filePathA = filesA[i];
    const std::string & filePathB = filesB[i];
    if (filePathA != filePathB)
      return false;
  }

  //content must be equals
  for(size_t i=0; i<filesA.size(); i++)
  {
    std::string filePathA = (folderA + "\\") + filesA[i];
    std::string filePathB = (folderB + "\\") + filesB[i];
    if (!ra::testing::IsFileEquals(filePathA.c_str(), filePathB.c_str()))
      return false;
  }

  return true;
}
