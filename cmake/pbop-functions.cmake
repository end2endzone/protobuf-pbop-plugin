# Get the location of protoc executable
get_property(PROTOC_LOCATION TARGET protobuf::protoc PROPERTY LOCATION)
get_filename_component(PROTOC_DIRECTORY ${PROTOC_LOCATION} DIRECTORY)
if(NOT DEFINED PROTOC_LOCATION)
    message(FATAL_ERROR "protoc compiler not found!")
endif() 
message("Found protoc location: " ${PROTOC_LOCATION} )


# Get the location of include directories for libprotobuf
get_target_property(PROTOBUF_INCLUDE_DIRS protobuf::libprotobuf INTERFACE_INCLUDE_DIRECTORIES)
if(NOT DEFINED PROTOBUF_INCLUDE_DIRS)
    message(FATAL_ERROR "protocol buffers include directory not found!")
endif() 
message("Found protobuf include directory: " ${PROTOBUF_INCLUDE_DIRS} )





#! pbop_generate_output_files : Generates the output files from a list of proto files.
#
# \arg:proto_files A list of all proto files.
# \arg:output_dir Output directory where the generated files must be generated.
# \arg:generated_files Name of the destination variable that will contains the list of the generated files.
#
function(pbop_generate_output_files proto_files output_dir generated_files)
  unset(LOCAL_GENERATED_FILES)
  
  foreach(PROTO_FILE ${proto_files})
    # Get the filename (without extension) of the proto file. ie: 'addressbook'
    get_filename_component(PROTO_FILENAME_WE ${PROTO_FILE} NAME_WE)

    # Define the output files absolute path
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pb.h)
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pb.cc)
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pbop.pb.h)
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pbop.pb.cc)
  endforeach()
  
  # Copy generated output files to the calling scope
  set(${generated_files} ${LOCAL_GENERATED_FILES} PARENT_SCOPE)
endfunction()



#! pbop_add_prebuild_target : Creates a prebuild target that runs the pbop plugin for a list of proto files.
#
# \arg:source_target_name The name of the target that contains the given proto files.
# \arg:prebuid_target_name The name of the prebuild target that generates files for target `source_target_name`.
# \arg:proto_files The list of proto files.
# \arg:output_dir Output directory where the generated files must be generated.
#
function(pbop_add_prebuild_target source_target_name prebuid_target_name proto_files output_dir)  
  foreach(PROTO_FILE ${proto_files})
    # Get the filename of the proto file. ie: addressbook.proto
    get_filename_component(PROTO_FILENAME ${PROTO_FILE} NAME)
    #message("PROTO_FILENAME=" ${PROTO_FILENAME})

    # Get the filename (without extension) of the proto file. ie: 'addressbook'
    get_filename_component(PROTO_FILENAME_WE ${PROTO_FILE} NAME_WE)
    #message("PROTO_FILENAME_WE=" ${PROTO_FILENAME_WE})

    # Get the directory of the proto file.
    get_filename_component(PROTO_DIRECTORY ${PROTO_FILE} DIRECTORY)
    #message("PROTO_DIRECTORY=" ${PROTO_DIRECTORY})

    # Define the output files absolute path
    unset(LOCAL_GENERATED_FILES)
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pb.h)
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pb.cc)
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pbop.pb.h)
    list(APPEND LOCAL_GENERATED_FILES ${output_dir}/${PROTO_FILENAME_WE}.pbop.pb.cc)
    
    # Execute 'addressbook.proto' and output to ${output_dir}
    add_custom_command(
      OUTPUT ${LOCAL_GENERATED_FILES}
      DEPENDS ${proto_file}
      # Warning: CMake treats ; character differently. They must be escaped to prevent issues
      COMMENT "Executing pbop plugin for ${PROTO_FILENAME}..."
      COMMAND echo $<TARGET_FILE:protobuf::protoc> --cpp_out=${output_dir} --plugin=protoc-gen-pbop=$<TARGET_FILE:protobuf-pbop-plugin> --pbop_out=${output_dir} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${PROTO_DIRECTORY}\;${output_dir} ${PROTO_FILENAME}
      COMMAND      $<TARGET_FILE:protobuf::protoc> --cpp_out=${output_dir} --plugin=protoc-gen-pbop=$<TARGET_FILE:protobuf-pbop-plugin> --pbop_out=${output_dir} --proto_path=.\;${PROTOBUF_INCLUDE_DIRS}\;${PROTO_DIRECTORY}\;${output_dir} ${PROTO_FILENAME}
      COMMAND echo done.
    )
        
    # Fix warnings when building on Windows  
    if (WIN32)
      # On Windows, with proobuf v3.5.1.1, the following warnings are always displayed
      #  google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
      #  google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
      #  google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
      #  google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)

      foreach(GENERATED_FILE ${LOCAL_GENERATED_FILES})
        set(pos -1)
        string(FIND "${GENERATED_FILE}" ".pb.cc" pos)
        if ("${pos}" GREATER -1)
          message("Disabling warning C4146 and warning C4800 for file '${GENERATED_FILE}'.")
          set_source_files_properties(${GENERATED_FILE} PROPERTIES COMPILE_FLAGS "/wd\"4146\" /wd\"4800\"")
        endif()
      endforeach()
    endif()
    
  endforeach()
    
  add_custom_target(${prebuid_target_name} DEPENDS ${LOCAL_GENERATED_FILES})
  add_dependencies(${source_target_name} ${prebuid_target_name})
  
endfunction()