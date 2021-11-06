cmake_minimum_required(VERSION 3.10)

project(webcom VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# simplyfile (external)
include_directories(SYSTEM external/simplyfile/src)
add_library(simplyfile
    external/simplyfile/src/simplyfile/Timer.cpp
    external/simplyfile/src/simplyfile/ThreadTime.cpp
    external/simplyfile/src/simplyfile/SerialPort.cpp
    external/simplyfile/src/simplyfile/Process.cpp
    external/simplyfile/src/simplyfile/INotify.cpp
    external/simplyfile/src/simplyfile/FileDescriptor.cpp
    external/simplyfile/src/simplyfile/Epoll.cpp
    external/simplyfile/src/simplyfile/socket/Host.cpp
    external/simplyfile/src/simplyfile/socket/Socket.cpp
)
target_link_libraries(simplyfile
)


# cndl (external)
include_directories(SYSTEM external/cndl/src)
add_library(cndl
    external/cndl/src/cndl/base64.cpp
    external/cndl/src/cndl/ConnectionHandler.cpp
    external/cndl/src/cndl/ProtocolHandler.cpp
    external/cndl/src/cndl/Dispatcher.cpp
    external/cndl/src/cndl/HttpProtocol.cpp
    external/cndl/src/cndl/Request.cpp
    external/cndl/src/cndl/Response.cpp
    external/cndl/src/cndl/Route.cpp
    external/cndl/src/cndl/Server.cpp
    external/cndl/src/cndl/Websocket.cpp
)
target_link_libraries(cndl
    simplyfile
)

# fmt (external)
add_subdirectory(external/fmt)


add_subdirectory(external/flattensObjectsNeatly)
include_directories(external/flattensObjectsNeatly/src)

# Executable PROJECT_NAME
include_directories(src)
