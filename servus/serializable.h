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

#ifdef SERVUS_USE_CXX03
#  include <boost/function/function0.hpp>
#  include <boost/shared_ptr.hpp>
#else
#  include <functional> // function
#  include <memory> // shared_ptr
#endif

namespace servus
{

/** Interface for serializable objects */
class Serializable
{
public:
    Serializable();
    virtual ~Serializable();

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
    SERVUS_API virtual uint128_t getTypeIdentifier() const;

    /**
     * Update this serializable from its binary representation.
     * @return true on success, false on error.
     */
    SERVUS_API bool fromBinary( const Data& data );
    SERVUS_API bool fromBinary( const void* data, const size_t size );

    /**
     * Get a binary representation of this object.
     *
     * The returned data is not thread safe, that is, it should not be modified
     * until the caller of this method has completed its execution.
     *
     * @return the binary representation of this object.
     */
    SERVUS_API Data toBinary() const;

    /**
     * Update this serializable from its JSON representation.
     * @return true on success, false on error.
     */
    SERVUS_API bool fromJSON( const std::string& json );

    /** @return the JSON representation of this serializable. */
    SERVUS_API std::string toJSON() const;
    //@}

    /** @name Change Notifications */
    //@{
    /** Callbacks for change notifications. */
#ifdef SERVUS_USE_CXX03
    typedef boost::function< void() > DeserializedCallback;
    typedef boost::function< void() > SerializeCallback;
#else
    typedef std::function< void() > DeserializedCallback;
    typedef std::function< void() > SerializeCallback;
#endif

    /**
     * Register a function called after the object has been updated remotely
     * (via a subscriber, a http server, loading from file...).
     * Only one callback is supported at the moment, to deregister the callback,
     * call this function with a 'nullptr' (or 0) parameter.
     *
     * @throw if a DeserializedCallback is already registered and the specified
     * callback is not 'nullptr' (or 0)
     */
    SERVUS_API void registerDeserializedCallback(
            const DeserializedCallback& callback );

    /**
     * Register a function to be called when the serializable object is about
     * to be serialized.
     * Only one callback is supported at the moment, to deregister the callback,
     * call this function with a 'nullptr' (or 0) parameter.
     *
     * @throw if a SerializedCallback is already registered and the specified
     * callback is not 'nullptr' (or 0)
     */
    SERVUS_API void registerSerializeCallback(
            const SerializeCallback& callback );
    //@}

private:
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

    Serializable( const Serializable& );
    Serializable& operator=( const Serializable& );
#ifdef SERVUS_USE_CXX11
    Serializable( Serializable&& ) = delete;
    Serializable& operator=( Serializable&& ) = delete;
#endif

    class Impl;
    Impl* _impl;
};

}

#endif // SERVUS_SERIALIZABLE_H
