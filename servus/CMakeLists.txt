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

set(SERVUS_PUBLIC_HEADERS
  listener.h
  result.h
  serializable.h
  servus.h
  types.h
  uint128_t.h
  uri.h
  )

set(SERVUS_HEADERS
  avahi/servus.h
  dnssd/servus.h
  none/servus.h
  )

set(SERVUS_SOURCES
  md5/md5.cc
  serializable.cpp
  servus.cpp
  uint128_t.cpp
  uri.cpp
  )

list(APPEND SERVUS_LINK_LIBRARIES PRIVATE ${CMAKE_THREAD_LIBS_INIT})
if(MSVC)
  list(APPEND SERVUS_LINK_LIBRARIES ws2_32)
endif()
if(DNSSD_FOUND)
  list(APPEND SERVUS_LINK_LIBRARIES ${DNSSD_LIBRARIES})
endif()
if(AVAHI-CLIENT_FOUND)
  list(APPEND SERVUS_LINK_LIBRARIES ${avahi-client_LIBRARIES})
endif()

set(SERVUS_OMIT_LIBRARY_HEADER ON)
common_library(Servus)

if(Qt5Core_FOUND)
  add_subdirectory(qt)
endif()
