cmake_minimum_required(VERSION 3.10)

project(flattensObjectsNeatly VERSION 0.1)

find_package(fmt CONFIG REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# catch (external)
include_directories(SYSTEM external/catch/src)
add_library(catch
	external/catch/src/catch/catch.cpp
)
target_link_libraries(catch
)

include_directories(SYSTEM src)
# testBinaryFon
add_executable(testBinaryFon
	src/testBinaryFon/testBool.cpp
	src/testBinaryFon/testClasses.cpp
	src/testBinaryFon/testContainer.cpp
	src/testBinaryFon/testEnum.cpp
	src/testBinaryFon/testFloats.cpp
	src/testBinaryFon/testInt.cpp
)

target_link_libraries(testBinaryFon
	catch
	stdc++fs
)

# testYAMLFon
add_executable(testYAMLFon
	src/testYAMLFon/main.cpp
	src/testYAMLFon/testBool.cpp
	src/testYAMLFon/testClasses.cpp
	src/testYAMLFon/testContainer.cpp
	src/testYAMLFon/testEnum.cpp
	src/testYAMLFon/testFloats.cpp
	src/testYAMLFon/testInt.cpp
	src/testYAMLFon/testPointers.cpp
	src/testYAMLFon/testSharedPointers.cpp
	src/testYAMLFon/testYaml.cpp
	src/testYAMLFon/testTuple.cpp
)

target_link_libraries(testYAMLFon
	catch
	yaml-cpp
	stdc++fs
)
