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

void addApplicationPath(const char * iPath);

void eraseToken(std::string & iString, const std::string & iToken);

bool hasMacroInFile(const char * iFilePath, std::string & oError);
bool hasMacroInFiles(const std::vector<std::string> & iFilesPath, std::string & oError);

size_t findFirst(const std::string & iValue, const std::vector<char> & iTokens);

std::string findWordName(const std::string & iBuffer);

bool isFolderEquals(const std::string & folderA, const std::string & folderB);
