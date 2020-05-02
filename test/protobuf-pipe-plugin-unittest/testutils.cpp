#include "testutils.h"
#include "gtesthelper.h"
#include "protobuf_locator.h"
#include <algorithm>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h> // For GetModuleFilename, GetVersionEx



static const char * PROTOBUF_DCOM_PLUGIN_NAME = "protobuf-dcom-plugin";



std::string getPluginName()
{
  return PROTOBUF_DCOM_PLUGIN_NAME;
}

std::string getPluginFileName()
{
  std::string path = getPluginFilePath();
  std::string filename = getFilenameFromPath(path);
  return filename;
}

std::string getPluginFilePath()
{
  std::string path = getExecutableFilePath();

  //replace test executable name by plugin executable name
  static const std::string plugin_name = PROTOBUF_DCOM_PLUGIN_NAME;
  static const std::string   test_name = "protobuf-dcom-plugin_unittest";
  replaceString(path, test_name, plugin_name);

  return path;
}

std::string getTestOutDir()
{
  gTestHelper & hlp = gTestHelper::getInstance();

  std::string outdir;
  outdir.append(getExecutableDir());
  outdir.append("\\");
  outdir.append(hlp.getTestSuiteName());
  outdir.append("Out");
  return outdir;
}

std::string getTestProtoFilePath()
{
  gTestHelper & hlp = gTestHelper::getInstance();

  std::string outdir;
  outdir.append(getExecutableDir());
  outdir.append("\\");
  outdir.append(hlp.getTestSuiteName());
  outdir.append(".proto");
  return outdir;
}

std::string getTestProtoPath()
{
  std::string outdir;
  outdir.append(getProtobufIncludeDirectory());
  outdir.append(";");
  outdir.append(getExecutableDir());
  outdir.append(";");
  outdir.append(getTestOutDir());
  return outdir;
}

std::string getEnvironementVariable(const char * iName)
{
  const char * value = getenv(iName);
  if (value != NULL)
  {
    return std::string(value);
  }
  return std::string();
}

void addApplicationPath(const char * iPath)
{
  std::string oldPath = getEnvironementVariable("PATH");

  std::string newPath;
  newPath.append("PATH=");
  newPath.append(iPath);
  newPath.append(";");
  newPath.append(oldPath);

  _putenv(newPath.c_str());
}

void setApplicationPath(const char * iFullPath)
{
  std::string newPath;
  newPath.append("PATH=");
  newPath.append(iFullPath);

  _putenv(newPath.c_str());
}

std::string getExecutableFilePath()                           
{                                                                 
  std::string path;                                               
  char buffer[MAX_PATH] = {0};                                    
  HMODULE hModule = NULL;                                         
  if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,           
          (LPCSTR) __FUNCTION__,   
          &hModule))                                              
  {                                                               
      int ret = GetLastError();                                   
      return std::string();                                       
  }                                                               
  /*get the path of this EXE*/                                    
  GetModuleFileName(hModule, buffer, sizeof(buffer));             
  if (buffer[0] != '\0')                                          
  {                                                               
    path = buffer;                                                
  }                                                               
  return path;                                                    
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

std::string getExecutableDir()
{
  std::string exec = getExecutableFilePath();

  std::string dir = getDirectoryFromPath(exec);
  return dir;
}

std::string getFilenameFromPath(const std::string & iPath)
{
  //search for the last path_separator location
#ifdef _WIN32
  const char path_separator = '\\';
#else
  const char path_separator = '/';
#endif

  std::string filename;

  size_t pos = iPath.find_last_of(path_separator);
  if (pos != std::string::npos)
  {
    //remove directory from path
    filename = iPath.substr(pos+1);
  }
  else
  {
    filename = iPath;
  }

  return filename;
}

std::string getDirectoryFromPath(const std::string & iPath)
{
  //search for the last path_separator location
#ifdef _WIN32
  const char path_separator = '\\';
#else
  const char path_separator = '/';
#endif

  std::string dir;

  size_t pos = iPath.find_last_of(path_separator);
  if (pos != std::string::npos)
  {
    //remove directory from path
    dir = iPath.substr(0, pos);
  }
  else
  {
    dir = iPath;
  }

  return dir;
}

std::string getFileExtensionFromPath(const std::string & iPath)
{
  std::string ext;

  size_t pos = iPath.find_last_of('.');
  if (pos != std::string::npos)
  {
    ext = iPath.substr(pos);
  }

  return ext;
}

bool hasMacroInFile(const char * iFilePath, std::string & oError)
{
  gTestHelper & hlp = gTestHelper::getInstance();
  size_t fileSize = hlp.getFileSize(iFilePath);

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

//https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
std::vector<std::string> get_all_files_names_within_folder(const std::string & folder)
{
  std::vector<std::string> names;
  std::string search_path = folder + "/*.*";
  WIN32_FIND_DATA fd; 
  HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
  if(hFind != INVALID_HANDLE_VALUE) { 
    do { 
      // read all (real) files in current folder
      // , delete '!' read other 2 default folder . and ..
      if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
        names.push_back(folder + "\\" + fd.cFileName);
      }
    }while(::FindNextFile(hFind, &fd)); 
    ::FindClose(hFind); 
  } 
  return names;
}

bool isFolderEquals(const std::string & folderA, const std::string & folderB)
{
  std::vector<std::string> filesA = get_all_files_names_within_folder(folderA);
  std::vector<std::string> filesB = get_all_files_names_within_folder(folderB);
  if (filesA.size() != filesB.size())
    return false;

  //remove the folder from each lists
  for(size_t i=0; i<filesA.size(); i++)
  {
    std::string & filePathA = filesA[i];
    replaceString(filePathA, folderA + "\\", "");
    
    std::string & filePathB = filesB[i];
    replaceString(filePathB, folderB + "\\", "");
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
  gTestHelper & hlp = gTestHelper::getInstance();
  for(size_t i=0; i<filesA.size(); i++)
  {
    std::string filePathA = (folderA + "\\") + filesA[i];
    std::string filePathB = (folderB + "\\") + filesB[i];
    if (!hlp.isFileEquals(filePathA.c_str(), filePathB.c_str()))
      return false;
  }

  return true;
}

//https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void replaceString(std::string& subject, const std::string& search, const std::string& replace)
{
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}