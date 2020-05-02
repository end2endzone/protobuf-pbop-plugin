#include "StreamPrinter.h"
#include <stdarg.h>
#include <shlobj.h>

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
  print( (unsigned char *)iValue.c_str(), iValue.size() );
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
