
set(SERVUS_PACKAGE_VERSION 0.1)
set(SERVUS_REPO_URL https://github.com/HBPVis/servus.git)
set(SERVUS_DEPENDS OPTIONAL DNSSD avahi-client Boost)
set(SERVUS_BOOST_COMPONENTS "unit_test_framework")
set(SERVUS_DEB_DEPENDS libboost-test-dev avahi-daemon libavahi-client-dev)

if(CI_BUILD_COMMIT)
  set(SERVUS_REPO_TAG ${CI_BUILD_COMMIT})
else()
  set(SERVUS_REPO_TAG master)
endif()
set(SERVUS_FORCE_BUILD ON)
set(SERVUS_SOURCE ${CMAKE_SOURCE_DIR})
