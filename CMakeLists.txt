# Copyright (c) 2015, Human Brain Project
#                     Juan Hernando <jhernando@fi.upm.es>

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Servus)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
include(GitExternal)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_ABI 1)

set(SERVUS_DESCRIPTION "A small set of network oriented utilities in C++ including a zeroconf implementation.")
set(COMMON_PROJECT_DOMAIN eu.humanbrainproject)

include(Common)

include(FindPackages)

add_subdirectory(servus)
add_subdirectory(tests)

include(CPackConfig)
include(DoxygenRule) # must be after all targets

