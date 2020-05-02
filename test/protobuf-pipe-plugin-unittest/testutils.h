#pragma once

#include <string>
#include <vector>

#define ASSERT_EXEC(cmdline) {int returncode = system(cmdline); ASSERT_EQ(0, returncode) << "Command line failed: " << cmdline;}

std::string getPluginName();
std::string getPluginFileName();
std::string getPluginFilePath();

std::string getTestOutDir();
std::string getTestProtoFilePath();
std::string getTestProtoPath();

std::string getEnvironementVariable(const char * iName);
void addApplicationPath(const char * iPath);
void setApplicationPath(const char * iFullPath);

void eraseToken(std::string & iString, const std::string & iToken);

std::string getExecutableFilePath();
std::string getExecutableDir();

std::string getFilenameFromPath(const std::string & iPath);
std::string getDirectoryFromPath(const std::string & iPath);
std::string getFileExtensionFromPath(const std::string & iPath);

bool hasMacroInFile(const char * iFilePath, std::string & oError);
bool hasMacroInFiles(const std::vector<std::string> & iFilesPath, std::string & oError);

size_t findFirst(const std::string & iValue, const std::vector<char> & iTokens);

std::string findWordName(const std::string & iBuffer);

std::vector<std::string> get_all_files_names_within_folder(const std::string & folder);

bool isFolderEquals(const std::string & folderA, const std::string & folderB);

void replaceString(std::string& subject, const std::string& search, const std::string& replace);
