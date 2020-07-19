# Define include dir for the library
set(LIB_PBOP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Define the *.proto files and their outputs
set(PROTO_FILES
  ${LIB_PBOP_INCLUDE_DIR}/pbop/pbop.proto
)
set(PROTO_GENERATED_FILES
  ${CMAKE_CURRENT_BINARY_DIR}/pbop.pb.cc
  ${CMAKE_CURRENT_BINARY_DIR}/pbop.pb.h
)

set(LIBPROTOBUFPBOPPLUGIN_INCLUDE_FILES
  ${LIB_PBOP_INCLUDE_DIR}/pbop/BufferedConnection.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Connection.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/CriticalSection.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Events.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Mutex.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/pbop.proto
  ${LIB_PBOP_INCLUDE_DIR}/pbop/PipeConnection.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/ReadWriteLock.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/ScopeLock.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Server.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Service.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Status.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Thread.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/ThreadBuilder.h
  ${LIB_PBOP_INCLUDE_DIR}/pbop/Types.h
)

add_library(pbop
  ${PBOP_EXPORT_HEADER}
  ${PBOP_VERSION_HEADER}
  ${PBOP_CONFIG_HEADER}
  #${PROTO_FILES}
  ${PROTO_GENERATED_FILES}
  ${LIBPROTOBUFPBOPPLUGIN_INCLUDE_FILES}
  BufferedConnection.cpp
  CriticalSection.cpp
  Events.cpp
  Mutex.cpp
  pbop.cpp
  pbop.h
  PipeConnection.cpp
  ReadWriteLock.cpp
  ScopeLock.cpp
  Server.cpp
  Status.cpp
)

# Show all proto files in a common folder
source_group("Proto Files" FILES ${PROTO_FILES})
source_group("Generated Files" FILES ${PROTO_GENERATED_FILES})


# Define a prebuild step of pbop
# Execute *.proto files and output to CMAKE_CURRENT_BINARY_DIR
foreach(PROTO_FILE ${PROTO_FILES})
  # Get the filename of the proto file. ie: addressbook.proto
  get_filename_component(PROTO_FILENAME ${PROTO_FILE} NAME)

  add_custom_command(
    OUTPUT ${PROTO_GENERATED_FILES}
    DEPENDS ${PROTO_FILES}
    # Warning: CMake treats ; character differently. They must be escaped to prevent issues
    COMMENT "Executing protoc for ${PROTO_FILE}..."
    COMMAND echo $<TARGET_FILE:protobuf::protoc> --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${LIB_PBOP_INCLUDE_DIR}/pbop ${PROTO_FILE}
    COMMAND      $<TARGET_FILE:protobuf::protoc> --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${LIB_PBOP_INCLUDE_DIR}/pbop ${PROTO_FILE}
    COMMAND echo done.
  )
endforeach()
add_custom_target(pbop-prebuild DEPENDS ${PROTO_GENERATED_FILES})
add_dependencies(pbop pbop-prebuild)


# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(pbop PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for exported code.
target_include_directories(pbop
  PUBLIC
    $<INSTALL_INTERFACE:${PBOP_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${LIB_PBOP_INCLUDE_DIR}    # For repository's include directory
    ${CMAKE_CURRENT_BINARY_DIR}             # For 'pbop.pb.h'
)
target_link_libraries(pbop PRIVATE protobuf::libprotobuf )

if (WIN32)
  # On Windows, with protobuf v3.5.1.1, the following warnings are always displayed
  #  google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
  #  google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
  #  google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
  #  google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)

  message("Disabling warning C4146 and warning C4800 for file '${CMAKE_CURRENT_BINARY_DIR}/pbop.pb.cc'.")
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/pbop.pb.cc PROPERTIES COMPILE_FLAGS "/wd\"4146\" /wd\"4800\"")
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pbop.pb.h
              DESTINATION ${PBOP_INSTALL_INCLUDE_DIR}/pbop)

install(TARGETS pbop
        EXPORT pbop-targets
        ARCHIVE DESTINATION ${PBOP_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PBOP_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PBOP_INSTALL_BIN_DIR}
)
