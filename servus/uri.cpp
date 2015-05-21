/* Copyright (c) 2013-2015, Human Brain Project
 *                          Ahmet.Bilgili@epfl.ch
 *                    2015, Juan Hernando <jhernando@fi.upm.es>
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

#include "uri.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <cassert>
#include <sstream>

namespace servus
{
namespace
{
struct URIData
{
    URIData() : port( 0 ) {}

    bool operator == ( const URIData& rhs ) const
    {
        return (userinfo == rhs.userinfo &&
                host == rhs.host &&
                port == rhs.port &&
                path == rhs.path &&
                query == rhs.query &&
                fragment == rhs.fragment &&
                queryMap == rhs.queryMap);
    }

    std::string scheme;
    std::string userinfo;
    std::string host;
    uint16_t port;
    std::string path;
    std::string query;
    std::string fragment;
    URI::KVMap queryMap;
};
}

namespace detail
{
class uri_parse : public std::exception
{
public:
    uri_parse( const std::string& uri )
    {
        _error << "Error parsing URI string: " << uri << std::endl;
    }

    uri_parse( const uri_parse& excep )
    {
        _error << excep._error.str();
    }

    virtual ~uri_parse() throw() {}

    virtual const char* what() const throw() { return _error.str().c_str(); }

private:
    std::stringstream _error;
};

enum URIPart { SCHEME = 0, AUTHORITY, PATH, QUERY, FRAGMENT, HIERARCHY };

bool _parseURIPart( std::string& input, const URIPart& part,
                    std::string& output )
{
    const char requireFirst[] = { 0, 0, 0, '?', '#' };
    const char* const separators[] = { ":", "/?#", "?#", "#", "" };
    const char* const disallowed[] = { "/?#", 0, 0, 0, 0 };
    const bool needsSeparator[] = { true, false, false, false, false };
    const size_t skip[] = { 0, 0, 0, 1, 1 };
    const size_t postSkip[] = { 1, 0, 0, 0, 0 };
    const size_t pos = input.find_first_of( separators[part] );

    if( pos == std::string::npos )
    {
        if( needsSeparator[part] )
        {
            output = "";
            return true;
        }
    }
    else if ( disallowed[part] &&
              input.find_first_of( disallowed[part] ) < pos )
    {
        output = "";
        return true;
    }

    // If the separator is not the first character, assert that parts requiring
    // an initial character find it.
    assert( !requireFirst[part] || pos == 0 || input[0] == requireFirst[part] );

    // If the separator was found at pos == 0, the returned string is empty
    assert( input.size() >= skip[part] );
    output = input.substr( skip[part], pos - skip[part] );
    input = pos == std::string::npos ?
        "" : input.substr( pos + postSkip[part] );
    return true;
}

void _parseAuthority( URIData& data, const std::string& auth )
{
    const size_t atPos = auth.find_first_of( '@' );
    if( atPos != std::string::npos )
        data.userinfo = auth.substr( 0, atPos );
    const size_t hostPos =
        atPos == std::string::npos ? 0 : atPos + 1;
    const size_t colonPos =
        auth.find_first_of( ':', hostPos );
    if( colonPos != std::string::npos )
        data.port = std::stoi( auth.substr( colonPos + 1 ));
    // Works regardless of colonPos == npos
    data.host = auth.substr( hostPos, colonPos - hostPos );
    if( data.host.empty( ))
        throw std::invalid_argument("");
}

void _parseQueryMap( URIData& data )
{
    // parse query data into key-value pairs
    std::string query = data.query;
    while( !query.empty( ))
    {
        const size_t nextPair = query.find( ',' );
        if( nextPair == 0 )
        {
            query = query.substr( 1 );
            continue;
        }

        const std::string pair = query.substr( 0, nextPair );
        if( nextPair == std::string::npos )
            query.clear();
        else
            query = query.substr( nextPair + 1 );

        const size_t eq = pair.find( '=' );
        if( eq == std::string::npos || eq == 0 )
            continue;
        data.queryMap[ pair.substr( 0, eq ) ] = pair.substr( eq + 1 );
    }
}

void _tolower( std::string& str )
{
    std::transform( str.begin(), str.end(), str.begin(), ::tolower );
}

class URI
{
public:
    explicit URI( const std::string& uri )
    {
        if( uri.empty( ))
            return;

        try
        {
            _parseURI( uri );
        }
        catch( ... )
        {
            throw uri_parse( uri );
        }
    }

    URIData& getData() { return _uriData; }
    const URIData& getData() const { return _uriData; }

private:
    URIData _uriData;

    void _parseURI( const std::string& uri )
    {
        std::string input = uri;
        URIPart part = SCHEME;
        while( !input.empty( ))
        {
            switch(part)
            {
            case SCHEME:
                _parseURIPart( input, part, _uriData.scheme );
                _tolower( _uriData.scheme );
                if( !_uriData.scheme.empty( ) &&
                    ( !isalpha( _uriData.scheme[0] ) ||
                      _uriData.scheme.find_first_not_of(
                          "abcdefghijlkmnopqrstuvwxyz0123456789+-.", 1 ) !=
                      std::string::npos ))
                {
                    throw std::invalid_argument("");
                }
                part = _uriData.scheme == "file" || _uriData.scheme.empty() ?
                       PATH : HIERARCHY;
                // from http://en.wikipedia.org/wiki/File_URI_scheme:
                //  "file:///foo.txt" is okay, while "file://foo.txt"
                // is not, although some interpreters manage to handle
                // the latter. We are "some".
                if( _uriData.scheme == "file" && input.substr( 0, 2 ) == "//" )
                    input = input.substr( 2 );
                break;
            case HIERARCHY:
                // Distinguishing <path> from <authority path>
                if( input.substr( 0, 2 ) == "//" )
                {
                    part = AUTHORITY;
                    input = input.substr( 2 );
                }
                else
                    part = PATH;
                break;
            case AUTHORITY:
            {
                std::string authority;
                _parseURIPart( input, part, authority );
                _parseAuthority( _uriData, authority );
                part = PATH;
                break;
            }
            case PATH:
                _parseURIPart( input, part, _uriData.path );
                part = QUERY;
                break;
            case QUERY:
                _parseURIPart( input, part, _uriData.query );
                _parseQueryMap( _uriData );
                part = FRAGMENT;
                break;
            case FRAGMENT:
                _parseURIPart( input, part, _uriData.fragment );
                break;
            }
        }
    }
};

}

URI::URI()
    : _impl( new detail::URI( std::string( )))
{
}

URI::URI( const std::string &uri )
   : _impl( new detail::URI( uri ) )
{
}

URI::URI( const char* uri )
    : _impl( new detail::URI( std::string( uri )))
{
}

URI::URI( const URI& from )
    : _impl( new detail::URI( *from._impl ))
{
}
servus::URI::~URI()
{
    delete _impl;
}

URI& URI::operator = ( const URI& rhs )
{
    if( this != &rhs )
        *_impl = *rhs._impl;
    return *this;
}

bool URI::operator == ( const URI& rhs ) const
{
    return this == &rhs || ( _impl->getData() == rhs._impl->getData( ));
}

bool URI::operator != ( const URI& rhs ) const
{
    return !( *this == rhs );
}

const std::string &URI::getScheme() const
{
    return _impl->getData().scheme;
}

const std::string &URI::getHost() const
{
    return _impl->getData().host;
}

uint16_t URI::getPort() const
{
    return _impl->getData().port;
}

const std::string &URI::getUserinfo() const
{
    return _impl->getData().userinfo;
}

const std::string& URI::getPath() const
{
    return _impl->getData().path;
}

const std::string& URI::getQuery() const
{
    return _impl->getData().query;
}

const std::string &URI::getFragment() const
{
    return _impl->getData().fragment;
}

void URI::setScheme( const std::string& scheme )
{
    _impl->getData().scheme = scheme;
}

void URI::setHost( const std::string& host )
{
    _impl->getData().host = host;
}

void URI::setPort( const uint16_t port )
{
    _impl->getData().port = port;
}

URI::ConstKVIter URI::queryBegin() const
{
    return _impl->getData().queryMap.begin();
}

URI::ConstKVIter URI::queryEnd() const
{
    return _impl->getData().queryMap.end();
}

URI::ConstKVIter URI::findQuery( const std::string& key ) const
{
    return _impl->getData().queryMap.find( key );
}

void URI::addQuery( const std::string& key, const std::string& value )
{
    URIData& data = _impl->getData();

    data.queryMap[ key ] = value;
    data.fragment.clear();

    // Rebuild fragment string
    data.query.clear();
    for( const auto& pair : data.queryMap )
    {
        if( data.query.empty( ))
            data.query = pair.first + "=" + pair.second;
        else
            data.query += std::string( "," ) + pair.first + "=" + pair.second;
    }
}

}
