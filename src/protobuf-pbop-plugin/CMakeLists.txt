add_executable(protobuf-pbop-plugin
  ${PBOP_VERSION_HEADER}
  ${PBOP_CONFIG_HEADER}
  DebugPrinter.cpp
  DebugPrinter.h
  main.cpp
  PluginCodeGenerator.cpp
  PluginCodeGenerator.h
  StreamPrinter.cpp
  StreamPrinter.h
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(protobuf-pbop-plugin PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Manually get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)

# Define include directories for exported code.
target_include_directories(protobuf-pbop-plugin
  PUBLIC
    $<INSTALL_INTERFACE:${PBOP_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/pbop
)

add_dependencies(protobuf-pbop-plugin pbop)

target_link_libraries(protobuf-pbop-plugin PRIVATE pbop protobuf::libprotobuf protobuf::libprotoc)

install(TARGETS protobuf-pbop-plugin
        EXPORT pbop-targets
        ARCHIVE DESTINATION ${PBOP_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PBOP_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PBOP_INSTALL_BIN_DIR}
)
