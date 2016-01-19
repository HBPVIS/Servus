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

#include <servus/api.h>
#include <servus/types.h>
#include <functional> // function
#include <memory> // shared_ptr

namespace servus
{

/** Interface for serializable objects */
class Serializable
{
public:
    virtual ~Serializable() {}

    /** Pointer + size wrapper for binary serialization. */
    struct Data
    {
        Data() : size ( 0 ) {}
        std::shared_ptr< const void > ptr; //!< ptr to the binary serialization
        size_t size; //!< The size of the binary serialization
    };

    /** @return the fully qualified, demangled class name. */
    virtual std::string getTypeName() const = 0;

    /** @return the universally unique identifier of this serializable. */
    SERVUS_API virtual uint128_t getTypeIdentifier() const;

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

    /** @return the JSON representation of this object. */
    std::string toJSON() const { return _toJSON(); }

    /** Function for change notification. */
    typedef std::function< void() > ChangeFunc;

    /**
     * Set a new function called after the object has been updated.
     *
     * @return the previously set function.
     */
    SERVUS_API ChangeFunc setUpdatedFunction( const ChangeFunc& func );

    /**
     * Set a new function called when a request has been received.
     *
     * Invoked before the object is published.
     * @return the previously set function.
     */
    SERVUS_API ChangeFunc setRequestedFunction( const ChangeFunc& func );

    /** @internal used by ZeroEQ to invoke updated function */
    SERVUS_API void notifyUpdated() const;
    /** @internal used by ZeroEQ to invoke updated function */
    SERVUS_API void notifyRequested() const;

protected:
    /** @name API for serializable sub classes. */
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

private:
    ChangeFunc _updated;
    ChangeFunc _requested;
};

}

#endif // SERVUS_SERIALIZABLE_H
