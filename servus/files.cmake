# Copyright (c) 2015, Human Brain Project
#                     Juan Hernando <jhernando@fi.upm.es>

set(SERVUS_PUBLIC_HEADERS
  ${COMMON_INCLUDES}
  result.h
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
  ${COMMON_SOURCES}
  md5/md5.cc
  servus.cpp
  uint128_t.cpp
  uri.cpp
  )
