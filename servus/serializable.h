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

#ifndef SERVUS_SERIALIZABLE_H
#define SERVUS_SERIALIZABLE_H

#include "uint128_t.h"

#include <servus/api.h>
#include <servus/types.h>

#include <boost/signals2.hpp>
#ifdef SERVUS_USE_CXX03
#  include <boost/shared_ptr.hpp>
#else
#  include <memory> // shared_ptr
#endif

namespace servus
{

/** Interface for serializable objects */
class Serializable
{
public:
    virtual ~Serializable() {}

    /** @name Serialization methods */
    //@{
    /** Pointer + size wrapper for binary serialization. */
    struct Data
    {
        Data() : size ( 0 ) {}
#ifdef SERVUS_USE_CXX03
        boost::shared_ptr< const void > ptr; //!< ptr to the binary serialization
#else
        std::shared_ptr< const void > ptr; //!< ptr to the binary serialization
#endif
        size_t size; //!< The size of the binary serialization
    };

    /** @return the fully qualified, demangled class name. */
    virtual std::string getTypeName() const = 0;

    /** @return the universally unique identifier of this serializable. */
    SERVUS_API virtual uint128_t getTypeIdentifier() const
        { return make_uint128( getTypeName( )); }

    /**
     * Update this serializable from its binary representation.
     * @return true on success, false on error.
     */
    bool fromBinary( const Data& data )
        { return _fromBinary( data.ptr.get(), data.size ); }
    bool fromBinary( const void* data, const size_t size )
        { return _fromBinary( data, size ); }

    /**
     * Get a binary representation of this object.
     *
     * The returned data is not thread safe, that is, it should not be modified
     * until the caller of this method has completed its execution.
     *
     * @return the binary representation of this object.
     */
    Data toBinary() const { return _toBinary(); }

    /**
     * Update this serializable from its JSON representation.
     * @return true on success, false on error.
     */
    bool fromJSON( const std::string& json ) { return _fromJSON( json ); }

    /** @return the JSON representation of this serializable. */
    std::string toJSON() const { return _toJSON(); }
    //@}

    /** @name Change Notifications */
    //@{
    typedef boost::signals2::signal< void() > ChangeSignal;

    //! Signal to emit after the object has been updated
    ChangeSignal updated;
    //! Signal to emit when a request has been received
    ChangeSignal requested;
    //@}

protected:
    /**
     * @name API for serializable sub classes.
     *
     * Endian-safe and 64-bit safe binary encoding is the responsability of the
     * subclass implementation, if needed.
     */
    //@{
    virtual bool _fromBinary( const void* /*data*/, const size_t /*size*/ )
        { throw std::runtime_error( "Binary deserialization not implemented" );}
    virtual Data _toBinary() const
        { throw std::runtime_error( "Binary serialization not implemented" ); }

    virtual bool _fromJSON( const std::string& /*json*/ )
        { throw std::runtime_error( "JSON deserialization not implemented" ); }
    virtual std::string _toJSON() const
        { throw std::runtime_error( "JSON serialization not implemented" ); }
    //@}
};

}

#endif // SERVUS_SERIALIZABLE_H
