# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

# Manually get the location of protoc
get_property(PROTOC_LOCATION TARGET protobuf::protoc PROPERTY LOCATION)

# Configure protobuf locator
set(PROTOBUF_LOCATOR_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/protobuf_locator.cpp)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/protobuf_locator.cpp.in ${PROTOBUF_LOCATOR_SOURCE} @ONLY)

add_executable(protobuf-pipe-plugin_unittest
  ${PROTOBUFPIPEPLUGIN_EXPORT_HEADER}
  ${PROTOBUFPIPEPLUGIN_VERSION_HEADER}
  ${PROTOBUFPIPEPLUGIN_CONFIG_HEADER}
  TestVoidMessages.proto
  gtesthelper.cpp
  gtesthelper.h
  main.cpp
  ${PROTOBUF_LOCATOR_SOURCE}
  protobuf_locator.cpp.in
  protobuf_locator.h
  TestBuildFull.cpp
  TestBuildFull.h
  TestBuildFull.proto
  TestBuildQuick.cpp
  TestBuildQuick.h
  TestBuildQuick.proto
  TestCalculator.cpp
  TestCalculator.h
  TestCalculator.proto
  TestDemo.cpp
  TestDemo.h
  TestDemo.proto
  TestPath.cpp
  TestPath.h
  TestPluginRun.cpp
  TestPluginRun.h
  TestPluginRun.proto
  TestProtoFunctions.cpp
  TestProtoFunctions.h
  TestStringFunctions.cpp
  TestStringFunctions.h
  testutils.cpp
  testutils.h
  TestVoidMessages.cpp
  TestVoidMessages.h
)

# Unit test projects requires to link with pthread if also linking with gtest
if(NOT WIN32)
  set(PTHREAD_LIBRARIES -pthread)
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(protobuf-pipe-plugin_unittest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

target_include_directories(protobuf-pipe-plugin_unittest 
  PRIVATE 
    ${GTEST_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/libPipe
    rapidassist
)
add_dependencies(protobuf-pipe-plugin_unittest protobuf-pipe-plugin libPipe)
target_link_libraries(protobuf-pipe-plugin_unittest PRIVATE libPipe ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} rapidassist)

# Define the proto files to process before building `protobuf-pipe-plugin_unittest`
set(OUTPUT_PROTO_FILES
  ${EXECUTABLE_OUTPUT_PATH}/TestBuildFull.proto
  ${EXECUTABLE_OUTPUT_PATH}/TestVoidMessages.proto
  ${EXECUTABLE_OUTPUT_PATH}/TestPluginRun.proto
  ${EXECUTABLE_OUTPUT_PATH}/TestDemo.proto
  ${EXECUTABLE_OUTPUT_PATH}/TestCalculator.proto
  ${EXECUTABLE_OUTPUT_PATH}/TestBuildQuick.proto
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
  #get_filename_component(filename_we ${output_file} NAME_WE)
  #get_filename_component(output_dir ${output_file} DIRECTORY)

  #Compute the input file's path
  set(input_file "${CMAKE_CURRENT_SOURCE_DIR}/${filename}")

  # message("Processing proto file: " ${filename})

  # Step 1: copy the file to the output directory
  add_custom_command(
    OUTPUT ${output_file}
    COMMAND ${CMAKE_COMMAND} -E copy
            ${input_file}
            ${output_file}
  )
endforeach()
add_custom_target(unittest-prebuild DEPENDS ${OUTPUT_PROTO_FILES})
add_dependencies(protobuf-pipe-plugin_unittest unittest-prebuild)


# Force Visual Studio to execute code in $(OutDir) when debugging from the IDE.
if (WIN32)
  file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/protobuf-pipe-plugin_unittest.vcxproj.user" 
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>     \
      <Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
      <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">
          <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
          <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
      </PropertyGroup>
      <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">
          <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
          <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
      </PropertyGroup>
      <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">
          <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
          <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
      </PropertyGroup>
      <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">
          <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
          <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
      </PropertyGroup>
      </Project>"
  )
endif()

install(TARGETS protobuf-pipe-plugin_unittest
        EXPORT protobufpipeplugin-targets
        ARCHIVE DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_BIN_DIR}
)
