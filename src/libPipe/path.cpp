#include "path.h"
#include "stringfunc.h"

void removeFileExtension(std::string & ioFilePath)
{
  std::string ext = getFileExtension(ioFilePath);
  if (!ext.empty())
  {
    //file has an extension
    size_t extPos = ioFilePath.size()-ext.size();
    ioFilePath.erase(ioFilePath.begin()+extPos, ioFilePath.end());
  }
}

std::string getFileName(const std::string & iFilePath)
{
  std::string output = iFilePath;
  size_t lastForwardSeparatorPos = iFilePath.find_last_of("/");
  size_t lastBackwardSeparatorPos = iFilePath.find_last_of("\\");
  size_t lastPos = getMaxIndexOf(lastForwardSeparatorPos, lastBackwardSeparatorPos);
  if (lastPos != std::string::npos)
  {
    //path contains a folder
    //only keep filename
    output.erase(output.begin(), output.begin()+(lastPos+1));
  }
  return output;
}

std::string getFileNameWithoutExtension(const std::string & iFilePath)
{
  std::string output = getFileName(iFilePath);
  removeFileExtension(output);
  return output;
}

std::string getFileExtension(const std::string & iFilePath)
{
  std::string output = iFilePath;
  size_t lastDotPos = output.find_last_of(".");
  if (lastDotPos != std::string::npos)
  {
    //file has an extension
    //remove everything before file extension
    output.erase(output.begin(), output.begin()+lastDotPos);
  }
  return output;
}

std::string getFolder(const std::string & iFilePath)
{
  std::string output = "";
  size_t lastForwardSeparatorPos = iFilePath.find_last_of("/");
  size_t lastBackwardSeparatorPos = iFilePath.find_last_of("\\");
  size_t lastPos = getMaxIndexOf(lastForwardSeparatorPos, lastBackwardSeparatorPos);
  if (lastPos != std::string::npos)
  {
    //path contains a folder
    //only keep folder
    output = iFilePath;
    output.erase(output.begin()+lastPos, output.end());
  }
  return output;
}
