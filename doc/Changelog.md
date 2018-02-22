
# Changelog {#Changelog}

# git master

* [82](https://github.com/HBPVis/Servus/pull/20):
  Expose servus_host and servus_port keys.
* [80](https://github.com/HBPVis/Servus/pull/80):
  Failsafe when Servus implementation can't be created and fallback to dummy.
* [77](https://github.com/HBPVis/Servus/pull/77):
  Add test driver implementation for Servus
* [76](https://github.com/HBPVis/Servus/pull/76):
  Add Serializable::Data::clone()

# Release 1.5.1 (22-05-2017)

* [72](https://github.com/HBPVis/Servus/pull/72):
  Removed legacy warning about ',' separator in URIs.
* [71](https://github.com/HBPVis/Servus/pull/71):
  Proper filtering for local services based on Avahi provided flags.
* [68](https://github.com/HBPVis/Servus/pull/68):
  Optimize uint128_t::getString()

# Release 1.5 (09-12-2016)

* [64](https://github.com/HBPVis/Servus/pull/64):
  Add Serializable::getSchema()
* [64](https://github.com/HBPVis/Servus/pull/64):
  Remove C++03 support
* [63](https://github.com/HBPVis/Servus/pull/63):
  Make Serializable copy- and moveable

# Release 1.4 (30-06-2016)

* [53](https://github.com/HBPVis/Servus/pull/53):
  Add notifyDeserialized and notifySerialize functions in serializable
* [47](https://github.com/HBPVis/Servus/pull/47):
  Use standard '&' separator instead of ',' for KV pairs in URI query [RFC6570]

# Release 1.3 (07-04-2016)

* [43](https://github.com/HBPVis/Servus/pull/43):
  Allow URI queries without value
* [34](https://github.com/HBPVis/Servus/pull/34):
  Serializable interface for zeq::http::Server
* [31](https://github.com/HBPVis/Servus/pull/31):
  Fix schema parsing in servus::URI: must be schema://, not only schema:
* [28](https://github.com/HBPVis/Servus/pull/28):
  Add a function to get the host corresponding to a given instance.

# Release 1.2 (02-11-2015)

* Added a new library ServusQt: Provides servus::qt::ItemModel which implements
  QAbstractItemModel to use it for any item view provided by Qt. Available only
  if Qt5::Core was found during build.
* Added servusBrowser tool to show usage of servus::qt::ItemModel. Available
  only if Qt5::Widgets was found during build.
* Fix deadlock on Windows in servus::make_UUID()

# Release 1.1 (07-07-2015)

* Added the function URI::getAuthority
* Added some URI setters.
* Build fixes for gcc 4.9.2 and MSVC.

# Release 1.0.1 (29-05-2015)

* Fixed URI parsing for URI without authority.

# Release 1.0 (21-05-2015)

* Initial version.
