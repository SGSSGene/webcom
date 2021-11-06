cmake_minimum_required(VERSION 3.12)

# check if this is the main project
if (NOT DEFINED PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS False)
endif ()

project(fon)

add_subdirectory(extern/Catch2)

add_library(fon INTERFACE)
target_include_directories(fon INTERFACE "src/")

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
)
target_link_libraries(testYAMLFon
    fon
    Catch2
    yaml-cpp
)

add_executable(exampleFon
    src/exampleFon/main.cpp
)
target_link_libraries(exampleFon
    fon
    yaml-cpp
)
