#pragma once

#include <string>

static const char * CRLF = "\x0D\x0A";
static const char * LF = "\x0A";

size_t getMaxIndexOf(size_t iPosA, size_t iPosB);

std::string capitalize(const std::string & iValue);
std::string capitalizeFirst(const std::string & iValue);
void stringReplace(std::string & iString, const char * iOld, const char * iNew);
std::string toString(const char * iBuffer, size_t iSize);
std::string toString(int value);
std::string uppercase(const std::string & value);
std::string lowercase(const std::string & value);

std::string& operator<<(std::string& str, const std::string & value);
std::string& operator<<(std::string& str, int value);
