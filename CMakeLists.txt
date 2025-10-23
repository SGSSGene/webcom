# SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
# SPDX-License-Identifier: CC0-1.0
cmake_minimum_required (VERSION 3.31)

project(webcom LANGUAGES CXX C
               DESCRIPTION "webcom -- helper classes to easily build mvc c++ <-> html/js systems")

include(cmake/CPM.cmake)
CPMAddPackage("gh:SGSSGene/cpmpack@1.0.2")
loadCPMPack("${CMAKE_CURRENT_SOURCE_DIR}/cpmpack.json")

if (PROJECT_IS_TOP_LEVEL)
    enable_testing()
endif()
