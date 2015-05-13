# Copyright (c) 2015, Human Brain Project
#                     Juan Hernando <jhernando@fi.upm.es>
#
# Change this number when adding tests to force a CMake run: 1

if(NOT BOOST_FOUND)
  return()
endif()

set(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Servus)

include(CommonCTest)

