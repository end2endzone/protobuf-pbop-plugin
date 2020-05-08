add_library(libProtobufPipePlugin
  ${PROTOBUFPIPEPLUGIN_EXPORT_HEADER}
  ${PROTOBUFPIPEPLUGIN_VERSION_HEADER}
  ${PROTOBUFPIPEPLUGIN_CONFIG_HEADER}
  DebugPrinter.cpp
  DebugPrinter.h
  libpipe.cpp
  libpipe.h
  StreamPrinter.cpp
  StreamPrinter.h
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(libProtobufPipePlugin PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for exported code.
target_include_directories(libProtobufPipePlugin
  PUBLIC
    $<INSTALL_INTERFACE:${PROTOBUFPIPEPLUGIN_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    rapidassist
)
target_link_libraries(libProtobufPipePlugin PRIVATE protobuf::libprotobuf rapidassist )

install(TARGETS libProtobufPipePlugin
        EXPORT protobufpipeplugin-targets
        ARCHIVE DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_BIN_DIR}
)
