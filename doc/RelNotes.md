Servus API Documentation {#mainpage}
=========

[TOC]

# Introduction {#Introduction}

Welcome to Servus, a small C++ network utility library that provides a zeroconf
API, uri parsing and UUIDs.

Servus 1.0 can be retrieved by cloning the [source
code](https://github.com/HBPVIS/servus). Please file a [Bug
Report](https://github.com/HBPVis/servus/issues) if you find any issues with
this release.

## Features {#Features}

Servus provides classes for:

* 128 bit UUIDs.
* A URI class to parse strings using generic syntax from [RFC3986](https://www.ietf.org/rfc/rfc3986.txt).
* Zeroconf announcing and browsing using Avahi or DNSSD. This is an optional
  feature available only if the dependencies are found.

- - -

## Changes {#Changes}

### Version 1.1

* Added the function URI::getAuthority

### Version 1.0.1

* Fixed URI parsing for URI without authority.

### Version 1.0

* Initial version.

- - -

# About {#About}

Servus is partially cross-platform, the only mandatory dependency is using
a C++11 compiler. Zeroconf will be available in those platforms were either
Avahi or DNSSD are available. Servus uses CMake to provide a
platform-independent build configuration. The following platforms and build
environments have been tested:

* Linux: Ubuntu 14.04, RHEL 6 using gcc 4.8.2

The following external, pre-installed optional dependencies are required:
* Boost.Test to build unit tests.
* Avahi (avahi-client) or DNSSD for zeroconf.

- - -

# Errata {#Errata}


