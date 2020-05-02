#pragma once

#include <google/protobuf/io/zero_copy_stream.h>

class StreamPrinter
{
public:
  StreamPrinter(google::protobuf::io::ZeroCopyOutputStream * iStream);
  virtual ~StreamPrinter();

  void print(const unsigned char * iValue, size_t iLength);
  void print(const std::string & iValue);
  void print(const char * iFormat, ...);

private:
  google::protobuf::io::ZeroCopyOutputStream * mStream;

};
