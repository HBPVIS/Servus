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

#include "serializable.h"

#include "uint128_t.h"

namespace servus
{
uint128_t Serializable::getTypeIdentifier() const
{
    return make_uint128( getTypeName( ));
}

Serializable::ChangeFunc Serializable::setUpdatedFunction(
    const ChangeFunc& func )
{
    const ChangeFunc old = _updated;
    _updated = func;
    return old;
}

Serializable::ChangeFunc Serializable::setRequestedFunction(
    const ChangeFunc& func )
{
    const ChangeFunc old = _requested;
    _requested = func;
    return old;
}

void Serializable::notifyUpdated() const
{
    if( _updated )
        _updated();
}

void Serializable::notifyRequested() const
{
    if( _requested )
        _requested();
}

}
