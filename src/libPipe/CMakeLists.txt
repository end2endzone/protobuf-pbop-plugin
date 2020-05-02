add_library(libPipe
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
set_target_properties(libPipe PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for exported code.
target_include_directories(libPipe
  PUBLIC
    $<INSTALL_INTERFACE:${PROTOBUFPIPEPLUGIN_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${PROTOBUF_INCLUDE_DIRS}
    rapidassist
)
target_link_libraries(libPipe PRIVATE protobuf::libprotobuf rapidassist )

install(TARGETS libPipe
        EXPORT protobufpipeplugin-targets
        ARCHIVE DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${PROTOBUFPIPEPLUGIN_INSTALL_BIN_DIR}
)
