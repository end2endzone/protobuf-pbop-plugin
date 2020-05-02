#include "StreamPrinter.h"

#include "rapidassist/strings.h"

#include <stdarg.h>

StreamPrinter::StreamPrinter(google::protobuf::io::ZeroCopyOutputStream * iStream) :
  mStream(iStream)
{
}

StreamPrinter::~StreamPrinter()
{
  if (mStream)
  {
    delete mStream;
    mStream = NULL;
  }
}

void StreamPrinter::print(const unsigned char * iValue, size_t iLength)
{
  size_t dumpIndex = 0;

  while(dumpIndex < iLength)
  {
    //get a buffer from the stream
    void * buffer = NULL;
    int bufferSize = 0;
    mStream->Next(&buffer, &bufferSize);

    if (buffer && bufferSize > 0)
    {
      //compute dumpsize
      size_t dumpSize = iLength - dumpIndex;
      if ((int)dumpSize > bufferSize)
        dumpSize = bufferSize;

      //dump
      const unsigned char * dumpData = &iValue[dumpIndex];
      memcpy(buffer, dumpData, dumpSize);

      if ((int)dumpSize < bufferSize)
      {
        int backupSize = bufferSize - dumpSize;
        mStream->BackUp(backupSize);
      }

      dumpIndex += dumpSize;
    }
  }
}

void StreamPrinter::print(const std::string & iValue)
{
#ifdef _WIN32
  //replace \n by \r\n
  static const std::string & windows_newline = "\r\n";
  static const std::string & unix_newline = "\n";

  //is the given buffer already properly formatted ?
  if (iValue.find(windows_newline) != std::string::npos)
  {
    //the given buffer already have \r\n format
    print( (unsigned char *)iValue.c_str(), iValue.size() );
  }
  else if (iValue.find(unix_newline) != std::string::npos)
  {
    //must replace \n by \r\n
    std::string tmp = iValue;
    ra::strings::Replace(tmp, unix_newline, windows_newline);
    print( (unsigned char *)tmp.c_str(), tmp.size() );
  }
  else
  {
    //no newline in given buffer, use as-is
    print( (unsigned char *)iValue.c_str(), iValue.size() );
  }
#else
  //use directly
  print( (unsigned char *)iValue.c_str(), iValue.size() );
#endif
}

void StreamPrinter::print(const char * iFormat, ...)
{
  std::string s;

  va_list args;
  va_start(args, iFormat);

  static const int BUFFER_SIZE = 102400;
  char buffer[BUFFER_SIZE];
  buffer[0] = '\0';
  vsprintf_s(buffer, BUFFER_SIZE, iFormat, args);
  s = buffer;

  va_end (args);

  print(s);
}
