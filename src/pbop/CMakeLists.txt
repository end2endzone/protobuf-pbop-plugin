# Define include dir for the library
set(LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

# Manually get the location of protoc
get_property(PROTOC_LOCATION TARGET protobuf::protoc PROPERTY LOCATION)

# Define the *.proto files and their outputs
set(PROTO_FILES
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/PipeMessages.proto
)
set(LIBPROTOBUFPIPEPLUGIN_INCLUDE_FILES
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/Connection.h
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/Events.h
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/PipeMessages.proto
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/Server.h
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/Service.h
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/Status.h
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop/Types.h
)

set(PROTO_GENERATED_FILES
  ${CMAKE_CURRENT_BINARY_DIR}/PipeMessages.pb.cc
  ${CMAKE_CURRENT_BINARY_DIR}/PipeMessages.pb.h
)

add_library(pbop
  ${PROTOBUFPIPEPLUGIN_EXPORT_HEADER}
  ${PROTOBUFPIPEPLUGIN_VERSION_HEADER}
  ${PROTOBUFPIPEPLUGIN_CONFIG_HEADER}
  #${PROTO_FILES}
  ${PROTO_GENERATED_FILES}
  ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_FILES}
  Connection.cpp
  Events.cpp
  pbop.cpp
  pbop.h
  Server.cpp
  Status.cpp
)

# Show all proto files in a common folder
source_group("Proto Files" FILES ${PROTO_FILES})
source_group("Generated Files" FILES ${PROTO_GENERATED_FILES})

# Execute 'PipeMessages.proto' and output to CMAKE_CURRENT_BINARY_DIR
add_custom_command(
  OUTPUT ${PROTO_GENERATED_FILES}
  DEPENDS ${PROTO_FILES}
  # Warning: CMake treats ; character differently. They must be escaped to prevent issues
  COMMENT "Executing protobuf-pipe-plugin for ${PROTO_FILES}..."
  COMMAND echo $<TARGET_FILE:protobuf::protoc> --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop ${PROTO_FILES}
  COMMAND      $<TARGET_FILE:protobuf::protoc> --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}/pbop ${PROTO_FILES}
  COMMAND echo done.
)

# Define a prebuild step of pbop
add_custom_target(pbop-prebuild DEPENDS ${PROTO_GENERATED_FILES})
add_dependencies(pbop pbop-prebuild)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(pbop PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for exported code.
target_include_directories(pbop
  PUBLIC
    $<INSTALL_INTERFACE:${PROTOBUFPIPEPLUGIN_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${LIBPROTOBUFPIPEPLUGIN_INCLUDE_DIR}    # For repository's include directory
    ${CMAKE_CURRENT_BINARY_DIR}             # For 'PipeMessages.pb.h'
)
target_link_libraries(pbop PRIVATE protobuf::libprotobuf )

if (WIN32)
  # On Windows, with protobuf v3.5.1.1, the following warnings are always displayed
  #  google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
  #  google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
  #  google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
  #  google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)

  message("Disabling warning C4146 and warning C4800 on Windows.")
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/PipeMessages.pb.cc PROPERTIES COMPILE_FLAGS "/wd\"4146\" /wd\"4800\"")
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PipeMessages.pb.h
              DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_INCLUDE_DIR}/pbop)

install(TARGETS pbop
        EXPORT protobufpipeplugin-targets
        ARCHIVE DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_BIN_DIR}
)