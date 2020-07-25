# CMakeLists.txt
cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
project(protobuf-pbop-plugin)

##############################################################################################################################################
# Standard CMake variables
##############################################################################################################################################

# Set a default build type if none was specified.
# See https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(   LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

##############################################################################################################################################
# CMake properties
##############################################################################################################################################
MESSAGE( STATUS "PROJECT_NAME:             " ${PROJECT_NAME} )
MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( STATUS "PROJECT_BINARY_DIR:       " ${PROJECT_BINARY_DIR} )
MESSAGE( STATUS "PROJECT_SOURCE_DIR:       " ${PROJECT_SOURCE_DIR} )
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH:   " ${EXECUTABLE_OUTPUT_PATH} )
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:      " ${LIBRARY_OUTPUT_PATH} )
MESSAGE( STATUS "CMAKE_MODULE_PATH:        " ${CMAKE_MODULE_PATH} )
MESSAGE( STATUS "CMAKE_COMMAND:            " ${CMAKE_COMMAND} )
MESSAGE( STATUS "CMAKE_ROOT:               " ${CMAKE_ROOT} )
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE:  " ${CMAKE_CURRENT_LIST_FILE} )
MESSAGE( STATUS "CMAKE_CURRENT_LIST_LINE:  " ${CMAKE_CURRENT_LIST_LINE} )
MESSAGE( STATUS "CMAKE_INCLUDE_PATH:       " ${CMAKE_INCLUDE_PATH} )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH:       " ${CMAKE_LIBRARY_PATH} )
MESSAGE( STATUS "CMAKE_SYSTEM:             " ${CMAKE_SYSTEM} )
MESSAGE( STATUS "CMAKE_SYSTEM_NAME:        " ${CMAKE_SYSTEM_NAME} )
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION:     " ${CMAKE_SYSTEM_VERSION} )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR:   " ${CMAKE_SYSTEM_PROCESSOR} )

##############################################################################################################################################
# Global settings
##############################################################################################################################################

# Product version according to Semantic Versioning v2.0.0 https://semver.org/
SET(PBOP_VERSION_MAJOR 0)
SET(PBOP_VERSION_MINOR 2)
SET(PBOP_VERSION_PATCH 0)
set(PBOP_VERSION ${PBOP_VERSION_MAJOR}.${PBOP_VERSION_MINOR}.${PBOP_VERSION_PATCH})

# read license file
file(READ ${CMAKE_SOURCE_DIR}/LICENSE.h LICENSE)

# version.h file
set(PBOP_VERSION_HEADER ${CMAKE_BINARY_DIR}/include/pbop/version.h)
message("Generating ${PBOP_VERSION_HEADER}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/protobuf-pbop-plugin/version.h.in ${PBOP_VERSION_HEADER} )

# config.h file
set(PBOP_CONFIG_HEADER ${CMAKE_BINARY_DIR}/include/pbop/config.h)
message("Generating ${PBOP_CONFIG_HEADER}...")
if (BUILD_SHARED_LIBS)
  set(PBOP_BUILD_TYPE_CPP_DEFINE "#define PBOP_BUILT_AS_SHARED")
else()
  set(PBOP_BUILD_TYPE_CPP_DEFINE "#define PBOP_BUILT_AS_STATIC")
endif()
configure_file( ${CMAKE_SOURCE_DIR}/src/protobuf-pbop-plugin/config.h.in ${PBOP_CONFIG_HEADER} )
set(PBOP_BUILD_TYPE_CPP_DEFINE)

# Define installation directories
set(PBOP_INSTALL_BIN_DIR      "bin")
set(PBOP_INSTALL_LIB_DIR      "lib/pbop-${PBOP_VERSION}")
set(PBOP_INSTALL_INCLUDE_DIR  "include/pbop-${PBOP_VERSION}")
set(PBOP_INSTALL_CMAKE_DIR    ${PBOP_INSTALL_LIB_DIR}) # CMake files (*.cmake) should have the same destination as the library files. Some also prefers to use "cmake".

##############################################################################################################################################
# Project settings
##############################################################################################################################################

# Build options
option(PBOP_BUILD_TEST "Build all protobuf-pbop-plugin's unit tests" OFF)
option(PBOP_BUILD_DOC "Build documentation" OFF)
option(PBOP_BUILD_SAMPLES "Build protobuf-pbop-plugin samples" OFF)

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

# Define include directories for source code.
# The specified values will not be exported.
include_directories(${CMAKE_BINARY_DIR}/include     # for ${PBOP_VERSION_HEADER} and ${PBOP_CONFIG_HEADER} generated files.
)

##############################################################################################################################################
# Dependencies
##############################################################################################################################################
find_package(GTest REQUIRED) #rapidassist requires GTest
find_package(rapidassist REQUIRED)
find_package(protobuf 3.5.1 REQUIRED NO_CMAKE_SYSTEM_PATH NO_CMAKE_PATH NO_CMAKE_SYSTEM_PACKAGE_REGISTRY )

##############################################################################################################################################
# Includes
##############################################################################################################################################

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(pbop-functions) # define macros and functions for pbop

##############################################################################################################################################
# Subprojects
##############################################################################################################################################

add_subdirectory(src/pbop)
add_subdirectory(src/protobuf-pbop-plugin)

# unit tests
if(PBOP_BUILD_TEST)
  add_subdirectory(test/protobuf-pbop-plugin-unittest)
endif()

##############################################################################################################################################
# Generate doxygen documentation
# See https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
##############################################################################################################################################
if (PBOP_BUILD_DOC)
  # check if Doxygen is installed
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
 
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")
 
    # note the option ALL which allows to build the docs together with the application
    add_custom_target( pbop_doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)
endif()

##############################################################################################################################################
# Install
##############################################################################################################################################

# Install locations:   See https://unix.stackexchange.com/a/36874
#   On UNIX, installs to "/usr/local".
#   On Windows, installs to "C:\Program Files (x86)\${PROJECT_NAME}" or to "C:\Program Files\${PROJECT_NAME}" for 64 bit binaries

# Target config version verification file
configure_file(${CMAKE_SOURCE_DIR}/cmake/pbop-config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/pbop-config-version.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/pbop-config-version.cmake DESTINATION ${PBOP_INSTALL_CMAKE_DIR})

# Target config file
configure_file(${CMAKE_SOURCE_DIR}/cmake/pbop-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/pbop-config.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/pbop-config.cmake DESTINATION ${PBOP_INSTALL_CMAKE_DIR})

install(FILES ${CMAKE_SOURCE_DIR}/cmake/pbop-functions.cmake DESTINATION ${PBOP_INSTALL_CMAKE_DIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${PBOP_INSTALL_INCLUDE_DIR})

install(FILES ${PBOP_EXPORT_HEADER}
              ${PBOP_VERSION_HEADER}
              ${PBOP_CONFIG_HEADER}
              DESTINATION ${PBOP_INSTALL_INCLUDE_DIR}/pbop)

install(EXPORT pbop-targets DESTINATION ${PBOP_INSTALL_CMAKE_DIR})

##############################################################################################################################################
# Packaging
##############################################################################################################################################

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PBOP_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR "${PBOP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PBOP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PBOP_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "protobuf-pbop-plugin is a C++ open-source plugin for Google Protocol Buffers which provides inter-process communication (IPC) over Windows Named Pipes.")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "ZIP;NSIS")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)
