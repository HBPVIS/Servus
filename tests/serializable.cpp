/* Copyright (c) 2013-2016, Jafet.VillafrancaDiaz@epfl.ch
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

#define BOOST_TEST_MODULE servus_serializable
#include <boost/test/unit_test.hpp>

#include <servus/uint128_t.h>
#include <servus/serializable.h>

void dummyFunction(){}

class SerializableObject : public servus::Serializable
{
public:
    std::string getTypeName() const final { return "test::serializable"; }

    servus::uint128_t getTypeIdentifier() const final
    {
        return servus::make_uint128( getTypeName( ));
    }

private:
    bool _fromBinary( const void*, const size_t ) { return true; }
    bool _fromJSON( const std::string& ) { return true; }
};


BOOST_AUTO_TEST_CASE(serializable_types)
{
    SerializableObject obj;
    BOOST_CHECK_EQUAL( obj.getTypeName(), "test::serializable" );
    BOOST_CHECK_EQUAL( servus::make_uint128( obj.getTypeName( )),
                       obj.getTypeIdentifier( ));
}

BOOST_AUTO_TEST_CASE(serializable_registerSerialize)
{
    SerializableObject obj;
    servus::Serializable::SerializeCallback callback( dummyFunction );

    obj.registerSerializeCallback( callback );
    BOOST_CHECK_THROW( obj.registerSerializeCallback( callback ),
                       std::runtime_error ); // callback already registered

    BOOST_CHECK_NO_THROW( obj.registerSerializeCallback(
        servus::Serializable::SerializeCallback( )));
    BOOST_CHECK_NO_THROW( obj.registerSerializeCallback( callback ));

    BOOST_CHECK_THROW( obj.registerSerializeCallback( callback ),
                       std::runtime_error ); // callback already registered
}

BOOST_AUTO_TEST_CASE(serializable_registerDeserialized)
{
    SerializableObject obj;
    servus::Serializable::DeserializedCallback callback( dummyFunction );

    obj.registerDeserializedCallback( callback );
    BOOST_CHECK_THROW( obj.registerDeserializedCallback( callback ),
                       std::runtime_error ); // callback already registered

    BOOST_CHECK_NO_THROW( obj.registerDeserializedCallback(
        servus::Serializable::DeserializedCallback( )));
    BOOST_CHECK_NO_THROW( obj.registerDeserializedCallback( callback ));

    BOOST_CHECK_THROW( obj.registerDeserializedCallback( callback ),
                       std::runtime_error ); // callback already registered
}

BOOST_AUTO_TEST_CASE(serializable_binary)
{
    SerializableObject obj;

    // fromBinary implemented
    BOOST_CHECK_NO_THROW( obj.fromBinary( new float[3], 3 ));

    // default toBinary (unimplemented)
    BOOST_CHECK_THROW( obj.toBinary(), std::runtime_error );
}

BOOST_AUTO_TEST_CASE(serializable_json)
{
    SerializableObject obj;

    // fromJson implemented
    BOOST_CHECK_NO_THROW( obj.fromJSON( std::string( "testing..." )));

    // default toJson (unimplemented)
    BOOST_CHECK_THROW( obj.toJSON(), std::runtime_error );
}
