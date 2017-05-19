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

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Servus VERSION 1.5.1)
set(Servus_VERSION_ABI 5)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
include(GitExternal)

set(SERVUS_DESCRIPTION
  "C++ network oriented utilities including a zeroconf implementation")
set(SERVUS_MAINTAINER "Human Brain Project <HBPVis@googlegroups.com>")
set(SERVUS_LICENSE LGPL)
set(SERVUS_DEB_DEPENDS avahi-daemon libavahi-client-dev libboost-test-dev)

include(Common)

if(LINUX)
  common_find_package(avahi-client)
else()
  common_find_package(DNSSD)
endif()
common_find_package(Boost 1.51 COMPONENTS unit_test_framework)
common_find_package(Qt5Core)
common_find_package(Qt5Widgets)
common_find_package(Threads REQUIRED)
common_find_package_post()

if(Qt5Core_FOUND)
  set(SERVUS_DEPENDENT_LIBRARIES Qt5Core)
endif()

add_subdirectory(servus)
add_subdirectory(apps)
add_subdirectory(tests)

include(CPackConfig)
set(COMMON_PROJECT_DOMAIN eu.humanbrainproject)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
set(DOXYGEN_MAINPAGE_MD README.md)
include(DoxygenRule) # must be after all targets
