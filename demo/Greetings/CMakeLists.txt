cmake_minimum_required(VERSION 3.0)
project(greetings)

find_package(protobufpipeplugin REQUIRED)
find_package(protobuf 3.5.1 REQUIRED NO_CMAKE_SYSTEM_PATH NO_CMAKE_PATH NO_CMAKE_SYSTEM_PACKAGE_REGISTRY )

# Force a debug postfix if none specified.
# This allows publishing both release and debug binaries to the same location
# and it helps to prevent linking with the wrong library on Windows.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Prevents annoying warnings on MSVC
if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(   LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Define the *.proto files and their outputs
set(PROTO_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/greetings.proto
)
set(PROTO_GENERATED_FILES
  ${CMAKE_CURRENT_BINARY_DIR}/greetings.pb.cc
  ${CMAKE_CURRENT_BINARY_DIR}/greetings.pb.h
  ${CMAKE_CURRENT_BINARY_DIR}/greetings.pipe.pb.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/greetings.pipe.pb.h
)

# ==========================================================================================
#   library
# ==========================================================================================

add_library(greetings_proto STATIC
  ${PROTO_FILES}
  ${PROTO_GENERATED_FILES}
)

# Define include directories and linking libraries
target_include_directories(greetings_proto
  PUBLIC
    ${PROTOBUF_INCLUDE_DIRS}
    libProtobufPipePlugin
)
target_link_libraries(greetings_proto libProtobufPipePlugin protobuf::libprotobuf)


# ==========================================================================================
#   Client
# ==========================================================================================

add_executable(greetings_client
  src/client.cpp
)

# Define include directories and linking libraries
target_include_directories(greetings_client
  PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    libProtobufPipePlugin
)
target_link_libraries(greetings_client libProtobufPipePlugin protobuf::libprotobuf greetings_proto)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(greetings_client PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

add_dependencies(greetings_client greetings_proto)


# ==========================================================================================
#   Server
# ==========================================================================================

add_executable(greetings_server
  src/server.cpp
)

# Define include directories and linking libraries
target_include_directories(greetings_server
  PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    libProtobufPipePlugin
)
target_link_libraries(greetings_server libProtobufPipePlugin protobuf::libprotobuf greetings_proto)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(greetings_server PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

add_dependencies(greetings_server greetings_proto)


# ==========================================================================================
#   Prebuild steps for protobuf
# ==========================================================================================

# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

# Manually get the location of protoc
get_property(PROTOC_LOCATION TARGET protobuf::protoc PROPERTY LOCATION)
message("PROTOC_LOCATION=" ${PROTOC_LOCATION})

# Manually get the location of protobuf-pipe-plugin
get_property(PROTOBUF_PIPE_PLUGIN_LOCATION TARGET protobuf-pipe-plugin PROPERTY LOCATION)
message("PROTOBUF_PIPE_PLUGIN_LOCATION=" ${PROTOBUF_PIPE_PLUGIN_LOCATION})

# Execute 'greetings.proto' and output to CMAKE_CURRENT_BINARY_DIR
add_custom_command(
  OUTPUT ${PROTO_GENERATED_FILES}
  DEPENDS ${input_file}
  # Warning: CMake treats ; character differently. They must be escaped to prevent issues
  COMMAND echo $<TARGET_FILE:protobuf::protoc> --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-pipe=$<TARGET_FILE:protobuf-pipe-plugin> --pipe_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/greetings.proto
  COMMAND      $<TARGET_FILE:protobuf::protoc> --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-pipe=$<TARGET_FILE:protobuf-pipe-plugin> --pipe_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/greetings.proto
  COMMAND echo done.
)

# Define a prebuild step of libProtobufPipePlugin
add_custom_target(greetings_proto-prebuild DEPENDS ${PROTO_GENERATED_FILES})
add_dependencies(greetings_proto greetings_proto-prebuild)

if (WIN32)
  # On Windows, with protobuf v3.5.1.1, the following warnings are always displayed
  #  google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
  #  google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
  #  google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
  #  google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)

  message("Disabling warning C4146 and warning C4800 on Windows.")
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/greetings.pb.cc       PROPERTIES COMPILE_FLAGS "/wd\"4146\" /wd\"4800\"")
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/greetings.pipe.pb.cpp PROPERTIES COMPILE_FLAGS "/wd\"4146\" /wd\"4800\"")
endif()
