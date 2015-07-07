
set(SERVUS_PACKAGE_VERSION 1.1)
set(SERVUS_REPO_URL https://github.com/HBPVis/Servus.git)
set(SERVUS_DEPENDS hbpvis REQUIRED Threads OPTIONAL Boost DNSSD avahi-client)
set(SERVUS_OPTIONAL ON)
set(SERVUS_BOOST_COMPONENTS "unit_test_framework")
set(SERVUS_DEB_DEPENDS libboost-test-dev avahi-daemon libavahi-client-dev)
