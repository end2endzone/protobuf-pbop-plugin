cmake_minimum_required(VERSION 3.0)
project(greetings)

find_package(protobuf 3.5.1 REQUIRED NO_CMAKE_SYSTEM_PATH NO_CMAKE_PATH NO_CMAKE_SYSTEM_PACKAGE_REGISTRY )
find_package(pbop REQUIRED)

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
pbop_generate_output_files("${PROTO_FILES}" ${CMAKE_CURRENT_BINARY_DIR} PROTO_GENERATED_FILES)

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
    pbop
)
target_link_libraries(greetings_proto pbop protobuf::libprotobuf)


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
    pbop
)
target_link_libraries(greetings_client pbop protobuf::libprotobuf greetings_proto)

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
    pbop
)
target_link_libraries(greetings_server pbop protobuf::libprotobuf greetings_proto)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(greetings_server PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

add_dependencies(greetings_server greetings_proto)


# ==========================================================================================
#   Prebuild steps which runs protobuf pbop plugin for each *.proto files
# ==========================================================================================

pbop_add_prebuild_target(greetings_proto greetings_proto-prebuild "${PROTO_FILES}" ${CMAKE_CURRENT_BINARY_DIR})
