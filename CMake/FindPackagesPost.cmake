
# Copyright (c) 2014 Stefan.Eilemann@epfl.ch

# Prefer dnssd (Bonjour) on Apple, avahi on all other platforms.
if(APPLE)
  if(DNSSD_FOUND AND avahi-client_FOUND)
    message(STATUS "Disabling avahi over preferred dnssd implementation")
    set(avahi-client_FOUND)
  endif()
elseif(LINUX)
  message(STATUS "Disabling broken dnssd avahi implementation")
  set(DNSSD_FOUND)
endif()
