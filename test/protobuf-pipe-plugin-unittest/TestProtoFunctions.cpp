#include "TestProtoFunctions.h"
#include "protofunc.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

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
    title->set_label(FieldDescriptorProto_Label::FieldDescriptorProto_Label_LABEL_OPTIONAL);
    title->set_type(FieldDescriptorProto_Type::FieldDescriptorProto_Type_TYPE_STRING);
    title->set_name("title");
    title->set_number(book->field_size());

    FieldDescriptorProto* isbn = book->add_field();
    isbn->set_label(FieldDescriptorProto_Label::FieldDescriptorProto_Label_LABEL_OPTIONAL);
    isbn->set_type(FieldDescriptorProto_Type::FieldDescriptorProto_Type_TYPE_STRING);
    isbn->set_name("isbn");
    isbn->set_number(book->field_size());

    FieldDescriptorProto* category = book->add_field();
    category->set_label(FieldDescriptorProto_Label::FieldDescriptorProto_Label_LABEL_OPTIONAL);
    category->set_type_name(categoryenum->name());
    category->set_name("category");
    category->set_number(book->field_size());
  }

  return tmp;
}

TEST_F(TestProtoFunctions, testToProtoString)
{
  FileDescriptorProto book = getBookProtoFile();
  std::string protoString = toProtoString(book);
  
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
  const FileDescriptor * desc = buildFileDescriptor(pool, book);
  ASSERT_TRUE(desc != NULL);

  std::string protoString2 = toProtoString(*desc);
  ASSERT_EQ(protoString, protoString2);
}
