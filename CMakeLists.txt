cmake_minimum_required(VERSION 3.10)

project(webcom VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# uSockets (external)
include_directories(SYSTEM external/uSockets/src)
add_library(uSockets
	external/uSockets/src/context.c
	external/uSockets/src/loop.c
	external/uSockets/src/socket.c
	external/uSockets/src/crypto/openssl.c
	external/uSockets/src/eventing/epoll_kqueue.c
	external/uSockets/src/eventing/gcd.c
	external/uSockets/src/eventing/libuv.c
)
target_link_libraries(uSockets
)

include_directories(SYSTEM external/uWebSockets/src)


add_subdirectory(external/flattensObjectsNeatly)
include_directories(external/flattensObjectsNeatly/src)

# Executable PROJECT_NAME
include_directories(src)
