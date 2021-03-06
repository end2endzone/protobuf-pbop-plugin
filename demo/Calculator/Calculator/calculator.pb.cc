// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: calculator.proto

#include "calculator.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)
namespace calculus {
class AddRequestDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<AddRequest>
      _instance;
} _AddRequest_default_instance_;
class AddResponseDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<AddResponse>
      _instance;
} _AddResponse_default_instance_;
}  // namespace calculus
namespace protobuf_calculator_2eproto {
void InitDefaultsAddRequestImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::calculus::_AddRequest_default_instance_;
    new (ptr) ::calculus::AddRequest();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::calculus::AddRequest::InitAsDefaultInstance();
}

void InitDefaultsAddRequest() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsAddRequestImpl);
}

void InitDefaultsAddResponseImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  ::google::protobuf::internal::InitProtobufDefaultsForceUnique();
#else
  ::google::protobuf::internal::InitProtobufDefaults();
#endif  // GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
  {
    void* ptr = &::calculus::_AddResponse_default_instance_;
    new (ptr) ::calculus::AddResponse();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::calculus::AddResponse::InitAsDefaultInstance();
}

void InitDefaultsAddResponse() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &InitDefaultsAddResponseImpl);
}

::google::protobuf::Metadata file_level_metadata[2];

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::calculus::AddRequest, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::calculus::AddRequest, left_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::calculus::AddRequest, right_),
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::calculus::AddResponse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(::calculus::AddResponse, value_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::calculus::AddRequest)},
  { 7, -1, sizeof(::calculus::AddResponse)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::calculus::_AddRequest_default_instance_),
  reinterpret_cast<const ::google::protobuf::Message*>(&::calculus::_AddResponse_default_instance_),
};

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "calculator.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 2);
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\020calculator.proto\022\010calculus\")\n\nAddReque"
      "st\022\014\n\004left\030\001 \001(\005\022\r\n\005right\030\002 \001(\005\"\034\n\013AddRe"
      "sponse\022\r\n\005value\030\001 \001(\0052G\n\021CalculatorServi"
      "ce\0222\n\003Add\022\024.calculus.AddRequest\032\025.calcul"
      "us.AddResponseb\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 182);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "calculator.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_calculator_2eproto
namespace calculus {

// ===================================================================

void AddRequest::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int AddRequest::kLeftFieldNumber;
const int AddRequest::kRightFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

AddRequest::AddRequest()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_calculator_2eproto::InitDefaultsAddRequest();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:calculus.AddRequest)
}
AddRequest::AddRequest(const AddRequest& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&left_, &from.left_,
    static_cast<size_t>(reinterpret_cast<char*>(&right_) -
    reinterpret_cast<char*>(&left_)) + sizeof(right_));
  // @@protoc_insertion_point(copy_constructor:calculus.AddRequest)
}

void AddRequest::SharedCtor() {
  ::memset(&left_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&right_) -
      reinterpret_cast<char*>(&left_)) + sizeof(right_));
  _cached_size_ = 0;
}

AddRequest::~AddRequest() {
  // @@protoc_insertion_point(destructor:calculus.AddRequest)
  SharedDtor();
}

void AddRequest::SharedDtor() {
}

void AddRequest::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AddRequest::descriptor() {
  ::protobuf_calculator_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_calculator_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const AddRequest& AddRequest::default_instance() {
  ::protobuf_calculator_2eproto::InitDefaultsAddRequest();
  return *internal_default_instance();
}

AddRequest* AddRequest::New(::google::protobuf::Arena* arena) const {
  AddRequest* n = new AddRequest;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void AddRequest::Clear() {
// @@protoc_insertion_point(message_clear_start:calculus.AddRequest)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&left_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&right_) -
      reinterpret_cast<char*>(&left_)) + sizeof(right_));
  _internal_metadata_.Clear();
}

bool AddRequest::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:calculus.AddRequest)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // int32 left = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &left_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // int32 right = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(16u /* 16 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &right_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:calculus.AddRequest)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:calculus.AddRequest)
  return false;
#undef DO_
}

void AddRequest::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:calculus.AddRequest)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 left = 1;
  if (this->left() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->left(), output);
  }

  // int32 right = 2;
  if (this->right() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->right(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:calculus.AddRequest)
}

::google::protobuf::uint8* AddRequest::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:calculus.AddRequest)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 left = 1;
  if (this->left() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->left(), target);
  }

  // int32 right = 2;
  if (this->right() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->right(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:calculus.AddRequest)
  return target;
}

size_t AddRequest::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:calculus.AddRequest)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // int32 left = 1;
  if (this->left() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->left());
  }

  // int32 right = 2;
  if (this->right() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->right());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void AddRequest::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:calculus.AddRequest)
  GOOGLE_DCHECK_NE(&from, this);
  const AddRequest* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const AddRequest>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:calculus.AddRequest)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:calculus.AddRequest)
    MergeFrom(*source);
  }
}

void AddRequest::MergeFrom(const AddRequest& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:calculus.AddRequest)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.left() != 0) {
    set_left(from.left());
  }
  if (from.right() != 0) {
    set_right(from.right());
  }
}

void AddRequest::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:calculus.AddRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AddRequest::CopyFrom(const AddRequest& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:calculus.AddRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AddRequest::IsInitialized() const {
  return true;
}

void AddRequest::Swap(AddRequest* other) {
  if (other == this) return;
  InternalSwap(other);
}
void AddRequest::InternalSwap(AddRequest* other) {
  using std::swap;
  swap(left_, other->left_);
  swap(right_, other->right_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata AddRequest::GetMetadata() const {
  protobuf_calculator_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_calculator_2eproto::file_level_metadata[kIndexInFileMessages];
}


// ===================================================================

void AddResponse::InitAsDefaultInstance() {
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int AddResponse::kValueFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

AddResponse::AddResponse()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    ::protobuf_calculator_2eproto::InitDefaultsAddResponse();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:calculus.AddResponse)
}
AddResponse::AddResponse(const AddResponse& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  value_ = from.value_;
  // @@protoc_insertion_point(copy_constructor:calculus.AddResponse)
}

void AddResponse::SharedCtor() {
  value_ = 0;
  _cached_size_ = 0;
}

AddResponse::~AddResponse() {
  // @@protoc_insertion_point(destructor:calculus.AddResponse)
  SharedDtor();
}

void AddResponse::SharedDtor() {
}

void AddResponse::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AddResponse::descriptor() {
  ::protobuf_calculator_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_calculator_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const AddResponse& AddResponse::default_instance() {
  ::protobuf_calculator_2eproto::InitDefaultsAddResponse();
  return *internal_default_instance();
}

AddResponse* AddResponse::New(::google::protobuf::Arena* arena) const {
  AddResponse* n = new AddResponse;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void AddResponse::Clear() {
// @@protoc_insertion_point(message_clear_start:calculus.AddResponse)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  value_ = 0;
  _internal_metadata_.Clear();
}

bool AddResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:calculus.AddResponse)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // int32 value = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(8u /* 8 & 0xFF */)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &value_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:calculus.AddResponse)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:calculus.AddResponse)
  return false;
#undef DO_
}

void AddResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:calculus.AddResponse)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 value = 1;
  if (this->value() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->value(), output);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), output);
  }
  // @@protoc_insertion_point(serialize_end:calculus.AddResponse)
}

::google::protobuf::uint8* AddResponse::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:calculus.AddResponse)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 value = 1;
  if (this->value() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->value(), target);
  }

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:calculus.AddResponse)
  return target;
}

size_t AddResponse::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:calculus.AddResponse)
  size_t total_size = 0;

  if ((_internal_metadata_.have_unknown_fields() &&  ::google::protobuf::internal::GetProto3PreserveUnknownsDefault())) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        (::google::protobuf::internal::GetProto3PreserveUnknownsDefault()   ? _internal_metadata_.unknown_fields()   : _internal_metadata_.default_instance()));
  }
  // int32 value = 1;
  if (this->value() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->value());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void AddResponse::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:calculus.AddResponse)
  GOOGLE_DCHECK_NE(&from, this);
  const AddResponse* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const AddResponse>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:calculus.AddResponse)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:calculus.AddResponse)
    MergeFrom(*source);
  }
}

void AddResponse::MergeFrom(const AddResponse& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:calculus.AddResponse)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.value() != 0) {
    set_value(from.value());
  }
}

void AddResponse::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:calculus.AddResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AddResponse::CopyFrom(const AddResponse& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:calculus.AddResponse)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AddResponse::IsInitialized() const {
  return true;
}

void AddResponse::Swap(AddResponse* other) {
  if (other == this) return;
  InternalSwap(other);
}
void AddResponse::InternalSwap(AddResponse* other) {
  using std::swap;
  swap(value_, other->value_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata AddResponse::GetMetadata() const {
  protobuf_calculator_2eproto::protobuf_AssignDescriptorsOnce();
  return ::protobuf_calculator_2eproto::file_level_metadata[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace calculus

// @@protoc_insertion_point(global_scope)
