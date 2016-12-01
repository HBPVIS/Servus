[TOC]

# Introduction

Servus is a small C++ network utility library that provides a zeroconf
API, URI parsing and UUIDs.

Servus can be retrieved by cloning the
[source code](https://github.com/HBPVIS/servus). Please file a
[Bug Report](https://github.com/HBPVis/servus/issues) if you find any issues
with this release.

## Features

Servus provides classes for:

* 128 bit UUIDs
* An URI class to parse strings using generic syntax from
  [RFC3986](https://www.ietf.org/rfc/rfc3986.txt)
* Zeroconf announcement and browsing using Avahi or DNSSD
* Detailed @ref Changelog

# Building

Servus is a cross-platform library, the only mandatory dependency is a C++11
compiler. Zeroconf will be available in those platforms were either Avahi or
DNSSD are available, otherwise an empty dummy backend is used. Servus uses CMake
to provide a platform-independent build configuration. The following platforms
and build environments have been tested:

* Linux: Ubuntu 16.04, RHEL 6.8 (Makefile, Ninja)
* Windows: 7 (Visual Studio 2012)
* Mac OS X: 10.9 (Makefile, Ninja)

The following external, pre-installed optional dependencies are used:

* Boost.Test to build unit tests
* Avahi (avahi-client) or DNSSD (Apple Bonjour) for zeroconf
* Qt5 Core for servus::qt::ItemModel
* Qt5 Widgets for servusBrowser tool

Building from source is as simple as:

    git clone https://github.com/HBPVIS/Servus.git
    mkdir Servus/build
    cd Servus/build
    cmake -GNinja ..
    ninja
