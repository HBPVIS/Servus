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

class Serializable::Impl
{
public:
    void notifyDeserialized() const
    {
        if( deserialized )
            deserialized();
    }

    void notifySerialize() const
    {
        if( serialize )
            serialize();
    }

    Serializable::DeserializedCallback deserialized;
    Serializable::SerializeCallback serialize;
};

Serializable::Serializable()
    : _impl( new Serializable::Impl( ))
{}

Serializable::~Serializable()
{
    delete _impl;
}

uint128_t Serializable::getTypeIdentifier() const
{
    return make_uint128( getTypeName( ));
}

bool Serializable::fromBinary( const Data& data )
{
    return fromBinary( data.ptr.get(), data.size );
}

bool Serializable::fromBinary( const void* data, const size_t size )
{
    if( _fromBinary( data, size ))
    {
        _impl->notifyDeserialized();
        return true;
    }
    return false;
}

Serializable::Data Serializable::toBinary() const
{
    _impl->notifySerialize();
    return _toBinary();
}

bool Serializable::fromJSON( const std::string& json )
{
    if( _fromJSON( json ))
    {
        _impl->notifyDeserialized();
        return true;
    }
    return false;
}

std::string Serializable::toJSON() const
{
    _impl->notifySerialize();
    return _toJSON();
}

void Serializable::registerDeserializedCallback(
        const DeserializedCallback& callback )
{
    if( _impl->deserialized && callback )
        throw( std::runtime_error(
                "A DeserializedCallback is already registered. "
                "Only one is supported at the moment" ));

    _impl->deserialized = callback;
}

void Serializable::registerSerializeCallback(
        const SerializeCallback& callback )
{
    if( _impl->serialize && callback )
        throw( std::runtime_error( "A SerializeCallback is already registered. "
                                   "Only one is supported at the moment" ));

    _impl->serialize = callback;
}

}
