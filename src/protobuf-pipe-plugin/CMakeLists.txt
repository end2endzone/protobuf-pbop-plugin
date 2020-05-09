add_executable(protobuf-pipe-plugin
  ${PROTOBUFPIPEPLUGIN_VERSION_HEADER}
  ${PROTOBUFPIPEPLUGIN_CONFIG_HEADER}
  DebugPrinter.cpp
  DebugPrinter.h
  main.cpp
  PluginCodeGenerator.cpp
  PluginCodeGenerator.h
  StreamPrinter.cpp
  StreamPrinter.h
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(protobuf-pipe-plugin PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

# Define include directories for exported code.
target_include_directories(protobuf-pipe-plugin
  PUBLIC
    $<INSTALL_INTERFACE:${PROTOBUFPIPEPLUGIN_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/libProtobufPipePlugin
    rapidassist
)

add_dependencies(protobuf-pipe-plugin libProtobufPipePlugin)

target_link_libraries(protobuf-pipe-plugin PRIVATE libProtobufPipePlugin rapidassist protobuf::libprotobuf protobuf::libprotoc)

install(TARGETS protobuf-pipe-plugin
        EXPORT protobufpipeplugin-targets
        ARCHIVE DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_BIN_DIR}
)
