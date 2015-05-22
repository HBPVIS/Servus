
set(SERVUS_PACKAGE_VERSION 1.0)
set(SERVUS_REPO_URL https://github.com/HBPVis/servus.git)
set(SERVUS_DEPENDS OPTIONAL DNSSD avahi-client Boost)
set(SERVUS_BOOST_COMPONENTS "unit_test_framework")
set(SERVUS_DEB_DEPENDS libboost-test-dev avahi-daemon libavahi-client-dev)
