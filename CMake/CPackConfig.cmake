# Copyright (c) HBP 2015 Stefan Eilemann <eile@eyescale.ch>
# All rights reserved. Do not distribute without further notice.

# General CPack configuration
# Info: http://www.itk.org/Wiki/CMake:Component_Install_With_CPack

set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libavahi-client3, avahi-daemon")
include(CommonCPack)
