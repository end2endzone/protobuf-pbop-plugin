![protobuf-pbop-plugin logo](docs/protobuf-pbop-plugin-splashscreen.jpg?raw=true)


# protobuf-pbop-plugin #
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/protobuf-pbop-plugin.svg)](https://github.com/end2endzone/protobuf-pbop-plugin/releases)


protobuf-pbop-plugin is a C++ open-source plugin for _Google Protocol Buffers_ which provides inter-process communication (IPC) over Windows Named Pipes.

The acronym PBOP is for **P**rotocol **B**uffers **O**ver **P**ipe (PBOP)



## Status ##

Build:

| Service | Build | Tests |
|----|-------|-------|
| AppVeyor | [![Build status](https://img.shields.io/appveyor/ci/end2endzone/protobuf-pbop-plugin/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/protobuf-pbop-plugin) | [![Tests status](https://img.shields.io/appveyor/tests/end2endzone/protobuf-pbop-plugin/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/protobuf-pbop-plugin/branch/master/tests) |


Statistics:

[![Statistics](https://buildstats.info/appveyor/chart/end2endzone/protobuf-pbop-plugin)](https://ci.appveyor.com/project/end2endzone/protobuf-pbop-plugin/branch/master)




# Purpose #

There is already multiple RPC options for implementing a client-server application. One of them is [gRPC](https://grpc.io/) which provides remote procedure call (RPC) support over sockets. In 99% of use case scenarios where an IPC or RPC solution is required, gRPC is the way to go.

However, gRPC have limitations on Windows platform. One of them is performance when communicating locally. Sockets have a relatively high overhead when communicating locally between processes because messages must go through the network routing protocols layer. On Unix systems, one can use Unix [domain socket](https://en.wikipedia.org/wiki/Unix_domain_socket) for low latency local communication since they don't go through this layer. Yet on Windows, there is no support for domain sockets.

For local inter-process communication, in situations where low latency is a requirement, [Named Pipes](https://en.wikipedia.org/wiki/Named_pipe) offer a great alternative over sockets. For examples, pipes are preferable over sockets in real-time application, for services that must process multiple queries in a short period.

protobuf-pbop-plugin was created for this purpose. It provides a simple solution for implementing low-latency, fast inter-process communication (IPC) solution over Named Pipes on Windows.




# Features #

The main features of protobuf-pbop-plugin are:

* Prebuild _Windows Named Pipe_ `Client` and `Server` base classes.
* Event based programming for the `Server` class: `Startup`, `Shutdown`, `Listening`, `Connection`, `Client-Created`, `Client-Disconnected`, `Client-Destroyed`, `Client-Error`.
* Rich error support: all utility functions return a `Status` class which encapsulates an error code and error message.
* Build on top of Google's Protocol Buffers library. Get serialization performance and robustness from a tested source.  


Note: protobuf-pbop-plugin **does not** support the following features:
* Client or Server _streaming RPCs_: service methods that uses or returns a stream instead of a message.
* Asynchronous IPC calls.




# Usage #

The following instructions show how to use protobuf-pbop-plugin.



## About Protocol Buffers ##

protobuf-pbop-plugin is a code generator plugin for [Protocol Buffers](https://developers.google.com/protocol-buffers) which is Google's library for serializing structured data. Once data is serialized, it can be transfered from a client to a server (or from a server back to a client).

Before continuing with the plugin, a good understanding of how to use protocol buffers is required. If you are already familiar with Protocol Buffers, you can skip this section.

If not, a good starting point is the official [c++ tutorial](https://developers.google.com/protocol-buffers/docs/cpptutorial).

Another good resource is the section [Working with Protocol Buffers](https://grpc.io/docs/what-is-grpc/introduction/#working-with-protocol-buffers) in gRPC documentation. Note that gRPC is also a Protocol Buffers plugin.



## About the generated code ##

When invoked, the plugin will generate c++ code for each [service](https://developers.google.com/protocol-buffers/docs/style#services) defined in your _proto_ file.

For each service, a class matching the service name will be generated with the following :

1) An interface called `StubInterface` that defines all the service methods.
2) A `Client` class that implements the service interface and have all the required functionality to establish and handle a pipe connection to a `pbop::Server`.
3) A `Service` class that implements the `StubInterface` and `pbop::Service` interface. This service class can be assigned to a `pbop::Server` which listens for incoming pipe connections.

See the example section for details.

The generated code have a dependency on the following libraries:
* protobuf-pbop-plugin
* Google's Protocol Buffers (protobuf)
* zLib



## Command line execution ##

On Windows, the compiler executable filename is `protoc.exe`. This executable's directory must be in the system's PATH environment variable to be used properly.

A Protocol Buffers plugin can be called by two different ways:

1) From the PATH environment variable.
2) From an absolute path.

This section explains the details of each method.



### Using PATH environment variable ###

This method is helpful if the plugin executable is in PATH environment variable.

The following command line, will execute the plugin and generate the required code:

`protoc.exe --<plugin_filename_executable>_out=<output_directory> --proto_path=<protobuf_include_directory>;<target_proto_directory>; <target_proto_filename>`

where
* `<plugin_filename_executable>` matches the filename without extension of the plugin executable.
* `<output_directory>` matches the output directory of the generated code.
* `<protobuf_include_directory>` matches the include directory of the protobuf library.
* `<target_proto_directory>` matches the directory of the target _proto_ file.
* `<target_proto_filename>` matches the path to the target _proto_ file.

For example:

`protoc.exe --protobuf-pbop-plugin_out=C:\Projets\demoplugin\output --proto_path=C:/Projets/third_parties/protobuf/install/include;C:\Projets\demoplugin\protos; C:\Projets\demoplugin\protos\demo.proto
`

Note: This method is working because the plugin's filename is `protobuf-pbop-plugin.exe`.



### Using plugin's absolute path ###

This method shall be used if the plugin's executable **is not** in PATH environment variable. The absolute path to the executable must be specified in the command line.

The following command line, will execute the plugin and generate the required code:

`protoc.exe --plugin=protoc-gen-<plugin_short_name>=<plugin_executable_path> --<plugin_short_name>_out=<output_directory> --proto_path=<protobuf_include_directory>;<target_proto_directory>; <target_proto_filename>`

where
* `<plugin_short_name>` matches a unique identifier for this plugin. Can be any word.
* `<plugin_executable_path>` matches the plugin's executable path.
* `<output_directory>` matches the output directory of the generated code.
* `<protobuf_include_directory>` matches the include directory of the protobuf library.
* `<target_proto_directory>` matches the directory of the target _proto_ file.
* `<target_proto_filename>` matches the path to the target _proto_ file.

For example:

`protoc.exe --plugin=protoc-gen-pbop=C:\Projets\demoplugin\bin\protobuf-pbop-plugin.exe --pbop_out=C:\Projets\demoplugin\output --proto_path=C:/Projets/third_parties/protobuf/install/include;C:\Projets\demoplugin\protos; C:\Projets\demoplugin\protos\demo.proto`

Note: In the previous example, the identifier `pbop` is used as the plugin's short name.



## Example: Greetings service ##

The following section show an actual example of using protobuf-pbop-plugin.

The `greetings.proto` file defines a single service called `Greeter`. The service has two service methods: `SayHello` and `SayGoodbye`. Each service method has their own "request" and "response" set of messages.



### greetings.proto file ### 

```protobuf
syntax = "proto3";

package greetings;

// The SayHello request message
message SayHelloRequest {
  string name = 1;
}

// The SayHello response message
message SayHelloResponse {
  string message = 1;
}

// The SayGoodbye request message
message SayGoodbyeRequest {
  string name = 1;
}

// The SayGoodbye response message
message SayGoodbyeResponse {
  string message = 1;
}

// The greeting service definition.
service Greeter {
  // Sends a greeting
  rpc SayHello (SayHelloRequest) returns (SayHelloResponse);
  
  // Sends a goodbye
  rpc SayGoodbye (SayGoodbyeRequest) returns (SayGoodbyeResponse);
}
```

### Creating a client ###

With the following, one can create a client that creates a pipe connection to a server:

```cpp
#include <stdio.h>
#include "greetings.pb.h"
#include "greetings.pbop.pb.h"
#include "pbop/Server.h"
#include "pbop/PipeConnection.h"

static const char * kPipeName = "\\\\.\\pipe\\greetings.pipe";

int main(int argc, char* argv[])
{
  printf("Running %s...\n", argv[0]);

  pbop::PipeConnection * connection = new pbop::PipeConnection();
  pbop::Status status = connection->Connect(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  greetings::Greeter::Client client(connection);

  greetings::SayHelloRequest request;
  greetings::SayHelloResponse response;

  request.set_name("bob");

  printf("%s says hello.\n", request.name().c_str());
  status = client.SayHello(request, response);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  printf("Message from server: %s\n", response.message().c_str());

  return 0;
}
```



### Creating a server ###

With the following, one can create a server that listens for a connection from a client:

```cpp
#include <stdio.h>
#include "greetings.pb.h"
#include "greetings.pbop.pb.h"
#include "pbop/Server.h"

static const char * kPipeName = "\\\\.\\pipe\\greetings.pipe";

class GreeterServiceImpl : public greetings::Greeter::Service
{
public:
  GreeterServiceImpl() {}
  virtual ~GreeterServiceImpl() {}

  pbop::Status SayHello(const greetings::SayHelloRequest & request, greetings::SayHelloResponse & response)
  {
    response.set_message("Greetings " + request.name());
    return pbop::Status::OK;
  }

  pbop::Status SayGoodbye(const greetings::SayGoodbyeRequest & request, greetings::SayGoodbyeResponse & response)
  {
    response.set_message("Farewell " + request.name());
    return pbop::Status::OK;
  }
};

int main(int argc, char* argv[])
{
  printf("Running %s...\n", argv[0]);

  GreeterServiceImpl * impl = new GreeterServiceImpl();

  pbop::Server server;
  server.RegisterService(impl);
  pbop::Status status = server.Run(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }
  return 0;
}
```




# Build #

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.




# Platform #

protobuf-pbop-plugin has been tested with the following platform:

*   Windows x86/x64




# Versioning #

This project use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/protobuf-pbop-plugin/tags).




# Authors #

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/protobuf-pbop-plugin/blob/master/AUTHORS) who participated in this project.




# License #

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.