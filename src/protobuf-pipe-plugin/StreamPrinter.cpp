/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

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

void StreamPrinter::Print(const unsigned char * iValue, size_t iLength)
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

void StreamPrinter::Print(const std::string & iValue)
{
#ifdef _WIN32
  //replace \n by \r\n
  static const std::string & windows_newline = "\r\n";
  static const std::string & unix_newline = "\n";

  //is the given buffer already properly formatted ?
  if (iValue.find(windows_newline) != std::string::npos)
  {
    //the given buffer already have \r\n format
    Print( (unsigned char *)iValue.c_str(), iValue.size() );
  }
  else if (iValue.find(unix_newline) != std::string::npos)
  {
    //must replace \n by \r\n
    std::string tmp = iValue;
    ra::strings::Replace(tmp, unix_newline, windows_newline);
    Print( (unsigned char *)tmp.c_str(), tmp.size() );
  }
  else
  {
    //no newline in given buffer, use as-is
    Print( (unsigned char *)iValue.c_str(), iValue.size() );
  }
#else
  //use directly
  print( (unsigned char *)iValue.c_str(), iValue.size() );
#endif
}

void StreamPrinter::Print(const char * iFormat, ...)
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

  Print(s);
}
