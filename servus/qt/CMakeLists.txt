# Copyright (c) 2015, Human Brain Project
#                     Daniel.Nachbaur@epfl.ch
#
# This file is part of Servus <https://github.com/HBPVIS/Servus>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

set(SERVUSQT_PUBLIC_HEADERS itemModel.h)
set(SERVUSQT_SOURCES itemModel.cpp)
set(SERVUSQT_LINK_LIBRARIES PUBLIC Qt5::Core Servus)
set(SERVUSQT_OMIT_LIBRARY_HEADER ON)
set(SERVUSQT_INCLUDE_NAME servus/qt)
set(SERVUSQT_NAMESPACE servusqt)
common_library(ServusQt)
