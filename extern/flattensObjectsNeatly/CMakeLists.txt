cmake_minimum_required(VERSION 3.12)

set(ROOT_PROJECT OFF)

# check if this is the main project
if (NOT DEFINED PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS False)
    set(ROOT_PROJECT ON)
endif ()

project(fon)

if (${ROOT_PROJECT})
    add_subdirectory(extern/Catch2)
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "src/")

add_executable(testBinaryFon
    src/testBinaryFon/main.cpp
    src/testBinaryFon/testBool.cpp
    src/testBinaryFon/testClasses.cpp
    src/testBinaryFon/testContainer.cpp
    src/testBinaryFon/testEnum.cpp
    src/testBinaryFon/testFloats.cpp
    src/testBinaryFon/testInts.cpp
    src/testBinaryFon/testPointers.cpp
)
target_link_libraries(testBinaryFon
    fon
    Catch2
)

add_executable(testYAMLFon
    src/testYAMLFon/main.cpp
    src/testYAMLFon/testBool.cpp
    src/testYAMLFon/testClasses.cpp
    src/testYAMLFon/testContainer.cpp
    src/testYAMLFon/testEnum.cpp
    src/testYAMLFon/testFloats.cpp
    src/testYAMLFon/testInts.cpp
    src/testYAMLFon/testPointers.cpp
    src/testYAMLFon/testYAML.cpp
)
target_link_libraries(testYAMLFon
    fon
    Catch2
    yaml-cpp
)

add_executable(testJSONFon
    src/testJSONFon/main.cpp
    src/testJSONFon/testBool.cpp
    src/testJSONFon/testClasses.cpp
    src/testJSONFon/testContainer.cpp
    src/testJSONFon/testEnum.cpp
    src/testJSONFon/testFloats.cpp
    src/testJSONFon/testInts.cpp
    src/testJSONFon/testPointers.cpp
    src/testJSONFon/testJSON.cpp
)
target_link_libraries(testJSONFon
    fon
    Catch2
    jsoncpp
)


add_executable(exampleFon
    src/exampleFon/main.cpp
)
target_link_libraries(exampleFon
    fon
    yaml-cpp
)
