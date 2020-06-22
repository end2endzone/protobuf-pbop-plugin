# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

# Manually get the location of protoc
get_property(PROTOC_LOCATION TARGET protobuf::protoc PROPERTY LOCATION)
get_filename_component(PROTOC_DIRECTORY ${PROTOC_LOCATION} DIRECTORY)

# Configure protobuf locator
set(PROTOBUF_LOCATOR_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/protobuf_locator.cpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/protobuf_locator.cpp.in ${PROTOBUF_LOCATOR_SOURCE} @ONLY)

add_executable(protobuf-pbop-plugin_unittest
  ${PBOP_EXPORT_HEADER}
  ${PBOP_VERSION_HEADER}
  ${PBOP_CONFIG_HEADER}
  main.cpp
  ${PROTOBUF_LOCATOR_SOURCE}
  protobuf_locator.cpp.in
  protobuf_locator.h
  TestPluginRun.cpp
  TestPluginRun.h
  TestPluginRun.proto
  TestProtoFunctions.cpp
  TestProtoFunctions.h
  TestServer.cpp
  TestServer.h
  TestUtils.cpp
  TestUtils.h
)

# Show all proto files in a common folder
source_group("Proto Files" FILES TestPluginRun.proto)

# Unit test projects requires to link with pthread if also linking with gtest
if(NOT WIN32)
  set(PTHREAD_LIBRARIES -pthread)
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(protobuf-pbop-plugin_unittest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(protobuf-pbop-plugin_unittest 
  PRIVATE 
    ${GTEST_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/pbop    # for pbop.h only
    rapidassist
)
add_dependencies(protobuf-pbop-plugin_unittest protobuf-pbop-plugin pbop)
target_link_libraries(protobuf-pbop-plugin_unittest PRIVATE pbop ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} rapidassist)

#------------------------------------------------
# Define a prebuild step for target 'protobuf-pbop-plugin_unittest'.
# Copies unit test *.proto files based on the following criteria:
#   1) At build time, copies the input file if output does not exists.
#   2) At build time, copies the input file if input file is modified.
#   Note: This step does not copy the input file if output file is modified.
#------------------------------------------------
set(OUTPUT_PROTO_FILES
  ${EXECUTABLE_OUTPUT_PATH}/TestPluginRun.proto
)
if (WIN32)
  # On Windows, we need to duplicate the output files for `Debug` and `Release` directories
  set(OUTPUT_PROTO_FILES_DEBUG)
  set(OUTPUT_PROTO_FILES_RELEASE)
  foreach(tmpfile ${OUTPUT_PROTO_FILES})
    string(REPLACE "${EXECUTABLE_OUTPUT_PATH}" "${EXECUTABLE_OUTPUT_PATH}/Debug"   tmp_debug   ${tmpfile})
    string(REPLACE "${EXECUTABLE_OUTPUT_PATH}" "${EXECUTABLE_OUTPUT_PATH}/Release" tmp_release ${tmpfile})
    set(OUTPUT_PROTO_FILES_DEBUG   ${OUTPUT_PROTO_FILES_DEBUG}   ${tmp_debug})
    set(OUTPUT_PROTO_FILES_RELEASE ${OUTPUT_PROTO_FILES_RELEASE} ${tmp_release})
  endforeach()
  set(OUTPUT_PROTO_FILES ${OUTPUT_PROTO_FILES_DEBUG} ${OUTPUT_PROTO_FILES_RELEASE})
endif()
foreach(output_file ${OUTPUT_PROTO_FILES})
  get_filename_component(filename ${output_file} NAME)

  #Compute the intput file's path
  set(input_file "${CMAKE_CURRENT_SOURCE_DIR}/${filename}")

  #Output a copy confirmation on console
  add_custom_command(
    OUTPUT ${output_file}
    DEPENDS ${input_file}
    COMMENT "Copying '${filename}' to '${output_file}'..."
    COMMAND ${CMAKE_COMMAND} -E copy
            ${input_file}
            ${output_file}
  )
  install(FILES ${input_file} DESTINATION ${PBOP_INSTALL_BIN_DIR})
endforeach()
add_custom_target(protobuf-pbop-plugin_unittest-prebuild DEPENDS ${OUTPUT_PROTO_FILES})
add_dependencies(protobuf-pbop-plugin_unittest protobuf-pbop-plugin_unittest-prebuild)

# Force Visual Studio to execute code in $(OutDir) when debugging from the IDE.
# and add protoc directory to PATH environment variable
if (WIN32)
  file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/protobuf-pbop-plugin_unittest.vcxproj.user" 
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>
      <Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
        <PropertyGroup>
          <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
          <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
          <LocalDebuggerEnvironment>PATH=${PROTOC_DIRECTORY};%PATH%</LocalDebuggerEnvironment>
        </PropertyGroup>
      </Project>"
  )
endif()

install(TARGETS protobuf-pbop-plugin_unittest
        EXPORT pbop-targets
        ARCHIVE DESTINATION ${PBOP_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PBOP_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PBOP_INSTALL_BIN_DIR}
)
