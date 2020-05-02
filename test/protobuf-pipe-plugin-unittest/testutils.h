#pragma once

#include <string>
#include <vector>

std::string getPluginShortName();
std::string getPluginFileName();
std::string getPluginFilePath();

std::string getTestProtoFilePath();
std::string getTestProtoPath();

void addApplicationPath(const char * iPath);

void eraseToken(std::string & iString, const std::string & iToken);

size_t findFirst(const std::string & iValue, const std::vector<char> & iTokens);

std::string findWordName(const std::string & iBuffer);

bool isFolderEquals(const std::string & folderA, const std::string & folderB);
