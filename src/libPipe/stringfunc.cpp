#include "stringfunc.h"

size_t getMaxIndexOf(size_t iPosA, size_t iPosB)
{
  size_t output = std::string::npos;
  if (iPosA != std::string::npos)
  {
    output = iPosA;
  }
  if (iPosB != std::string::npos)
  {
    if (output == std::string::npos)
    {
      output = iPosB;
    }
    else if (iPosB > output)
    {
      output = iPosB;
    }
  }
  return output;
}

std::string capitalize(const std::string & iValue)
{
  std::string copy = iValue;
  if (!copy.empty())
  {
    for(size_t i=0; i<iValue.size(); i++)
    {
      copy[i] = toupper(copy[i]);
    }
  }
  return copy;
}

std::string capitalizeFirst(const std::string & iValue)
{
  std::string copy = iValue;
  if (!copy.empty())
  {
    copy[0] = toupper(copy[0]);
  }
  return copy;
}

void stringReplace(std::string & iString, const char * iOld, const char * iNew)
{
  if (std::string(iOld) == "")
    return;

  size_t pos = iString.find(iOld);
  size_t oldLen = std::string(iOld).size();

  while(pos != std::string::npos)
  {
    iString.replace(iString.begin()+pos, iString.begin()+pos+oldLen, iNew);
    pos = iString.find(iOld);
  }
}

std::string toString(const char * iBuffer, size_t iSize)
{
  std::string output(iBuffer, iSize);
  return output;
}

std::string toString(int value)
{
  static const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  sprintf_s(buffer, BUFFER_SIZE, "%d", value);
  return std::string(buffer);
}

std::string uppercase(const std::string & value)
{
  std::string copy = value;
  for(size_t i=0; i<copy.size(); i++)
  {
    copy[i] = (char)toupper(copy[i]);
  }
  return copy;
}

std::string lowercase(const std::string & value)
{
  std::string copy = value;
  for(size_t i=0; i<copy.size(); i++)
  {
    copy[i] = (char)tolower(copy[i]);
  }
  return copy;
}

std::string& operator<<(std::string& str, const std::string & value)
{
  str.append(value);
  return str;
}

std::string& operator<<(std::string& str, int value)
{
  char buffer[1024];
  sprintf(buffer, "%d", value);
  str.append(buffer);
  return str;
}
