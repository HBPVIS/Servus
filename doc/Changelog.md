
# Changelog {#Changelog}

# git master {#master}

# Release 1.2 (02-11-2015) {#Release010200}

* Added a new library ServusQt: Provides servus::qt::ItemModel which implements
  QAbstractItemModel to use it for any item view provided by Qt. Available only
  if Qt5::Core was found during build.
* Added servusBrowser tool to show usage of servus::qt::ItemModel. Available
  only if Qt5::Widgets was found during build.
* Fix deadlock on Windows in servus::make_UUID()

# Release 1.1 (07-07-2015) {#Release010100}

* Added the function URI::getAuthority
* Added some URI setters.
* Build fixes for gcc 4.9.2 and MSVC.

# Release 1.0.1 (29-05-2015) {#Release010010}

* Fixed URI parsing for URI without authority.

# Release 1.0 (21-05-2015) {#Release010000}

* Initial version.
