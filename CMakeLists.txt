cmake_minimum_required(VERSION 3.10)

set(ROOT_PROJECT OFF)
# check if this is the main project
if (NOT DEFINED PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS False)
    set(ROOT_PROJECT ON)
endif ()


project(webcom VERSION 0.1)

if (${ROOT_PROJECT})
    # simplyfile (extern)
    add_library(simplyfile
        extern/simplyfile/Timer.cpp
        extern/simplyfile/ThreadTime.cpp
        extern/simplyfile/SerialPort.cpp
        extern/simplyfile/Process.cpp
        extern/simplyfile/INotify.cpp
        extern/simplyfile/FileDescriptor.cpp
        extern/simplyfile/Epoll.cpp
        extern/simplyfile/socket/Host.cpp
        extern/simplyfile/socket/Socket.cpp
    )
    target_include_directories(simplyfile PUBLIC extern)
    target_link_libraries(simplyfile
    )


    # cndl (extern)
    add_library(cndl
        extern/cndl/ConnectionHandler.cpp
        extern/cndl/DateStrHelper.cpp
        extern/cndl/Dispatcher.cpp
        extern/cndl/HttpProtocol.cpp
        extern/cndl/ProtocolHandler.cpp
        extern/cndl/Request.cpp
        extern/cndl/Response.cpp
        extern/cndl/Route.cpp
        extern/cndl/Server.cpp
        extern/cndl/StaticFileHandler.cpp
        extern/cndl/WSRoute.cpp
        extern/cndl/Websocket.cpp
        extern/cndl/base64.cpp
    )
    target_include_directories(cndl PUBLIC extern)
    target_link_libraries(cndl
        simplyfile
    )

    add_subdirectory(extern/fmt)
    add_subdirectory(extern/flattensObjectsNeatly)
    add_subdirectory(extern/flattensObjectsNeatly/extern/Catch2)

endif()

# Executable PROJECT_NAME
add_library(${PROJECT_NAME} INTERFACE
)
target_include_directories(${PROJECT_NAME} INTERFACE src)
target_link_libraries(${PROJECT_NAME} INTERFACE
    cndl
    fon
)


add_executable(exampleWebcom
    src/exampleWebcom/main.cpp
)
target_link_libraries(exampleWebcom
    cndl
    fon
    webcom
    yaml-cpp
    fmt
    crypto
    pthread
)

add_executable(exampleWebcom2
    src/exampleWebcom2/main.cpp
)
target_link_libraries(exampleWebcom2
    fon
    webcom
    yaml-cpp
    fmt
    crypto
)

add_executable(testWebcom
    src/testWebcom/main.cpp
)
target_link_libraries(testWebcom
    Catch2
    fon
    webcom
    yaml-cpp
    fmt
    crypto
)
