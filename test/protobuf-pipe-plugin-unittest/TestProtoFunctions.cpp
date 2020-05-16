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

#include "TestProtoFunctions.h"
#include "libpipe.h"

#ifdef _WIN32
//google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
//google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)
__pragma( warning(push) )
__pragma( warning(disable: 4800))
__pragma( warning(disable: 4146))
#endif //_WIN32

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

using namespace google::protobuf;

void TestProtoFunctions::SetUp()
{
}

void TestProtoFunctions::TearDown()
{
}

FileDescriptorProto getBookProtoFile()
{
  FileDescriptorProto tmp;
  tmp.set_name("book");
  tmp.set_package("booklibrary");

  //book message
  {
    DescriptorProto* book = tmp.add_message_type();
    book->set_name("Book");
    
    google::protobuf::EnumDescriptorProto * categoryenum = book->add_enum_type();
    categoryenum->set_name("Category");
    EnumValueDescriptorProto* adventure = categoryenum->add_value();
    adventure->set_name("ADVENTURE");
    adventure->set_number(categoryenum->value_size());
    EnumValueDescriptorProto* fiction = categoryenum->add_value();
    fiction->set_name("FICTION");
    fiction->set_number(categoryenum->value_size());
    EnumValueDescriptorProto* horror = categoryenum->add_value();
    horror->set_name("HORROR");
    horror->set_number(categoryenum->value_size());
    EnumValueDescriptorProto* fantasy = categoryenum->add_value();
    fantasy->set_name("FANTASY");
    fantasy->set_number(categoryenum->value_size());

    FieldDescriptorProto* title = book->add_field();
    title->set_label(/*FieldDescriptorProto_Label::*/FieldDescriptorProto_Label_LABEL_OPTIONAL);
    title->set_type(/*FieldDescriptorProto_Type::*/FieldDescriptorProto_Type_TYPE_STRING);
    title->set_name("title");
    title->set_number(book->field_size());

    FieldDescriptorProto* isbn = book->add_field();
    isbn->set_label(/*FieldDescriptorProto_Label::*/FieldDescriptorProto_Label_LABEL_OPTIONAL);
    isbn->set_type(/*FieldDescriptorProto_Type::*/FieldDescriptorProto_Type_TYPE_STRING);
    isbn->set_name("isbn");
    isbn->set_number(book->field_size());

    FieldDescriptorProto* category = book->add_field();
    category->set_label(/*FieldDescriptorProto_Label::*/FieldDescriptorProto_Label_LABEL_OPTIONAL);
    category->set_type_name(categoryenum->name());
    category->set_name("category");
    category->set_number(book->field_size());
  }

  return tmp;
}

TEST_F(TestProtoFunctions, testToProtoString)
{
  FileDescriptorProto book = getBookProtoFile();
  std::string protoString = libProtobufPipePlugin::ToProtoString(book);
  
  printf("%s\n", protoString.c_str());

  ASSERT_FALSE(protoString.empty());
  ASSERT_NE(protoString.find("message Book"), std::string::npos);
  ASSERT_NE(protoString.find("optional string title = 1"), std::string::npos);
  ASSERT_NE(protoString.find("optional string isbn = 2"), std::string::npos);
  ASSERT_NE(protoString.find("optional .booklibrary.Book.Category category = 3"), std::string::npos);
  ASSERT_NE(protoString.find("enum Category"), std::string::npos);
  ASSERT_NE(protoString.find("ADVENTURE = 1"), std::string::npos);
  ASSERT_NE(protoString.find("FICTION = 2"), std::string::npos);
  ASSERT_NE(protoString.find("HORROR = 3"), std::string::npos);
  ASSERT_NE(protoString.find("FANTASY = 4"), std::string::npos);

  DescriptorPool pool;
  const FileDescriptor * desc = libProtobufPipePlugin::BuildFileDescriptor(pool, book);
  ASSERT_TRUE(desc != NULL);

  std::string protoString2 = libProtobufPipePlugin::ToProtoString(*desc);
  ASSERT_EQ(protoString, protoString2);
}
