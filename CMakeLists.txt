# SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
# SPDX-License-Identifier: CC0-1.0
cmake_minimum_required (VERSION 3.30)

project(webcom LANGUAGES CXX C
               DESCRIPTION "webcom -- helper classes to easily build mvc c++ <-> html/js systems")

include(cmake/CPM.cmake)
CPMAddPackage("gh:SGSSGene/cpm.dependencies@1.0.0")
CPMLoadDependenciesFile("${CMAKE_CURRENT_SOURCE_DIR}/cpm.dependencies")

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE
    fon::fon
    cndl::cndl
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
    $<INSTALL_INTERFACE:include>
)


if (PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_executable(testWebcom
        src/testWebcom/main.cpp
    )
    target_link_libraries(testWebcom
        Catch2::Catch2WithMain
        webcom::webcom
        fmt::fmt
    )
    add_test(NAME testWebcom COMMAND testWebcom)

    add_executable(exampleWebcom
        src/exampleWebcom/main.cpp
    )
    target_link_libraries(exampleWebcom
        Catch2::Catch2WithMain
        webcom::webcom
        fmt::fmt
    )

    add_executable(exampleWebcom2
        src/exampleWebcom2/main.cpp
    )
    target_link_libraries(exampleWebcom2
        Catch2::Catch2WithMain
        webcom::webcom
        fmt::fmt
    )
endif()
