# Copyright (c) 2015, Human Brain Project
#                     Juan Hernando <jhernando@fi.upm.es>
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

include(files.cmake)

if(MSVC)
  list(APPEND SERVUS_LINK_LIBRARIES ws2_32)
endif()
if(DNSSD_FOUND)
  list(APPEND SERVUS_LINK_LIBRARIES ${DNSSD_LIBRARIES})
endif()
if(AVAHI-CLIENT_FOUND)
  list(APPEND SERVUS_LINK_LIBRARIES ${avahi-client_LIBRARIES})
endif()

set(SERVUS_OMIT_PROJECT_HEADER 1)
common_library(Servus)
source_group(\\ FILES CMakeLists.txt)
source_group(servus FILES ${SERVUS_PUBLIC_HEADERS} ${SERVUS_HEADERS}
  ${SERVUS_SOURCES})


