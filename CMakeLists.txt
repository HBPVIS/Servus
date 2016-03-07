# Copyright (c) 2015-2016, Human Brain Project
#                          Juan Hernando <jhernando@fi.upm.es>
#
# This file is part of Servus <https://github.com/HBPVIS/Servus>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Servus)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
include(GitExternal)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "3")
set(VERSION_PATCH "0")
set(VERSION_ABI 3)

set(SERVUS_DESCRIPTION
  "C++ network oriented utilities including a zeroconf implementation")
set(SERVUS_MAINTAINER "Human Brain Project <HBPVis@googlegroups.com>")
set(CPACK_PACKAGE_LICENSE LGPL)
set(COMMON_PROJECT_DOMAIN eu.humanbrainproject)

include(Common)

if(LINUX)
  common_package(avahi-client)
else()
  common_package(DNSSD)
endif()
if(COMMON_USE_CXX03)
  set(BOOST_COMPONENTS REQUIRED COMPONENTS unit_test_framework random system
    thread chrono)
else()
  set(BOOST_COMPONENTS COMPONENTS unit_test_framework)
endif()
common_package(Boost 1.51 ${BOOST_COMPONENTS})
common_package(Qt5Core)
common_package(Qt5Widgets)
common_package(Threads REQUIRED)
common_package_post()

if(Qt5Core_FOUND)
  set(SERVUS_DEPENDENT_LIBRARIES Qt5Core)
endif()

add_subdirectory(servus)
add_subdirectory(apps)
add_subdirectory(tests)

include(CPackConfig)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
set(DOXYGEN_MAINPAGE_MD README.md)
include(DoxygenRule) # must be after all targets
