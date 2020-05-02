#pragma once

#include <string>

std::string getFileName(const std::string & iFilePath);
std::string getFileNameWithoutExtension(const std::string & iFilePath);
std::string getFileExtension(const std::string & iFilePath);
std::string getFolder(const std::string & iFilePath);
void removeFileExtension(std::string & ioFilePath);
