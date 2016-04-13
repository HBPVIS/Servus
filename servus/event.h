/* Copyright (c) 2016, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 *
 * This file is part of Servus <https://github.com/HBPVIS/Servus>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef SERVUS_EVENT_H
#define SERVUS_EVENT_H

#include <servus/serializable.h> // base class

namespace servus
{

/** Interface for events */
class Event : private servus::Serializable
{
public:
    Event( const std::string& name, const Serializable::ChangeFunc& func )
        : _name( name )
        { setUpdatedFunction( func ); }

private:
    std::string getTypeName() const final { return _name; }

    std::string _name;
};

}

#endif // SERVUS_EVENT_H
