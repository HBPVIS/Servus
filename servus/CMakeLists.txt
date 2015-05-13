# Copyright (c) 2015, Human Brain Project
#                     Juan Hernando <jhernando@fi.upm.es>

include(files.cmake)

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


