/* Copyright (c) 2013-2014, ahmet.bilgili@epfl.ch
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

#define BOOST_TEST_MODULE servus_uri
#include <boost/test/unit_test.hpp>

#include <servus/uri.h>


BOOST_AUTO_TEST_CASE(test_uri_parts)
{
    const std::string uriStr =
        "http://bob@www.example.com:8080/path/?key=value,foo=bar#fragment";
    BOOST_REQUIRE_NO_THROW( servus::URI uri( uriStr ));
    servus::URI uri( uriStr );

    BOOST_CHECK_EQUAL( uri.getScheme(), "http" );
    BOOST_CHECK_EQUAL( uri.getHost(), "www.example.com" );
    BOOST_CHECK_EQUAL( uri.getUserinfo(), "bob" );
    BOOST_CHECK_EQUAL( uri.getPort(), 8080 );
    BOOST_CHECK_EQUAL( uri.getPath(), "/path/" );
    BOOST_CHECK_EQUAL( uri.getQuery(), "key=value,foo=bar" );
    BOOST_CHECK_EQUAL( uri.getFragment(), "fragment" );

    const servus::URI hostPortURI( "foo://hostname:12345" );
    BOOST_CHECK_EQUAL( hostPortURI.getScheme() ,  "foo" );
    BOOST_CHECK_EQUAL( hostPortURI.getHost() , "hostname" );
    BOOST_CHECK_EQUAL( hostPortURI.getPort(), 12345 );

    const servus::URI uppercaseURI( "FOO:" );
    BOOST_CHECK_EQUAL( uppercaseURI.getScheme(), "foo" );
}

BOOST_AUTO_TEST_CASE(test_setters)
{
    servus::URI uri;
    uri.setScheme( "foo" );
    BOOST_CHECK_EQUAL( uri.getScheme(), "foo" );
    uri.setHost( "host" );
    BOOST_CHECK_EQUAL( uri.getHost(), "host" );
    uri.setPort( 12345 );
    BOOST_CHECK_EQUAL( uri.getPort(), 12345 );
}

BOOST_AUTO_TEST_CASE(test_empty_uri)
{
    servus::URI empty;
    BOOST_CHECK( empty.getScheme().empty( ));
    BOOST_CHECK( empty.getHost().empty( ));
    BOOST_CHECK( empty.getUserinfo().empty( ));
    BOOST_CHECK_EQUAL( empty.getPort(), 0 );
    BOOST_CHECK( empty.getPath().empty( ));
    BOOST_CHECK( empty.getQuery().empty( ));
    BOOST_CHECK( empty.getFragment().empty( ));
}

BOOST_AUTO_TEST_CASE(test_file_uris)
{
    servus::URI file1( "/bla.txt" );
    BOOST_CHECK_EQUAL( file1.getPath(), "/bla.txt" );
    BOOST_CHECK( file1.getHost().empty( ));
    BOOST_CHECK( file1.getScheme().empty( ));
    BOOST_CHECK( file1.getQuery().empty( ));
    BOOST_CHECK( file1.getFragment().empty( ));

    servus::URI file2( "bla.txt" );
    BOOST_CHECK_EQUAL( file2.getPath(), "bla.txt" );
    BOOST_CHECK( file2.getHost().empty( ));
    BOOST_CHECK( file2.getScheme().empty( ));
    BOOST_CHECK( file2.getQuery().empty( ));
    BOOST_CHECK( file2.getFragment().empty( ));

    servus::URI file3( "file:///bla.txt" );
    BOOST_CHECK_EQUAL( file3.getPath(), "/bla.txt" );
    BOOST_CHECK( file3.getHost().empty( ));
    BOOST_CHECK_EQUAL( file3.getScheme(), "file" );
    BOOST_CHECK( file3.getQuery().empty( ));
    BOOST_CHECK( file3.getFragment().empty( ));

    servus::URI file4( "file://bla.txt" );
    BOOST_CHECK_EQUAL( file4.getPath(), "bla.txt" );
    BOOST_CHECK( file4.getHost().empty( ));
    BOOST_CHECK_EQUAL( file4.getScheme(), "file" );
    BOOST_CHECK( file4.getQuery().empty( ));
    BOOST_CHECK( file4.getFragment().empty( ));

    servus::URI file5( "scheme://bla.txt" );
    BOOST_CHECK_EQUAL( file5.getHost(), "bla.txt" );
    BOOST_CHECK( file5.getPath().empty( ));
    BOOST_CHECK_EQUAL( file5.getScheme(), "scheme" );
    BOOST_CHECK( file5.getQuery().empty( ));
    BOOST_CHECK( file5.getFragment().empty( ));

    servus::URI path1( "foo:/bla.txt" );
    BOOST_CHECK( path1.getHost().empty( ));
    BOOST_CHECK_EQUAL( path1.getPath(), "/bla.txt" );
    BOOST_CHECK_EQUAL( path1.getScheme(), "foo" );
    BOOST_CHECK( path1.getQuery().empty( ));
    BOOST_CHECK( path1.getFragment().empty( ));

    servus::URI path2( "foo:bla.txt" );
    BOOST_CHECK( path2.getHost().empty( ));
    BOOST_CHECK_EQUAL( path2.getPath(), "bla.txt" );
    BOOST_CHECK_EQUAL( path2.getScheme(), "foo" );
    BOOST_CHECK( path2.getQuery().empty( ));
    BOOST_CHECK( path2.getFragment().empty( ));
}

BOOST_AUTO_TEST_CASE(test_uri_query)
{
    const std::string uriStr =
        "http://bob@www.example.com:8080/path/?key=value,foo=bar#fragment";
    servus::URI uri( uriStr );

    BOOST_CHECK( uri.findQuery( "key" ) != uri.queryEnd( ));
    BOOST_CHECK( uri.findQuery( "foo" ) != uri.queryEnd( ));
    BOOST_CHECK( uri.findQuery( "bar" ) == uri.queryEnd( ));
    BOOST_CHECK_EQUAL( uri.findQuery( "key" )->second, "value" );
    BOOST_CHECK_EQUAL( uri.findQuery( "foo" )->second, "bar" );

    uri.addQuery( "hans", "dampf" );
    BOOST_CHECK_EQUAL( uri.findQuery( "key" )->second, "value" );
    BOOST_CHECK_EQUAL( uri.findQuery( "foo" )->second, "bar" );
    BOOST_CHECK_EQUAL( uri.findQuery( "hans" )->second, "dampf" );
    BOOST_CHECK( uri.getQuery().find( "hans=dampf" ) != std::string::npos );
}

BOOST_AUTO_TEST_CASE(test_uri_comparisons)
{
    const std::string uriStr =
        "http://bob@www.example.com:8080/path/?key=value,foo=bar#fragment";
    servus::URI uri( uriStr );

    BOOST_CHECK( uri == uri );
    BOOST_CHECK( uri == servus::URI( uriStr ));
    BOOST_CHECK( uri != servus::URI(
                           "http://bob@www.example.com:8080/path/?key=value" ));
    BOOST_CHECK( uri != servus::URI(
          "http://bob@www.example.com:8030/path/?key=value,foo=bar#fragment" ));
    BOOST_CHECK( uri != servus::URI(
                  "http://bob@foo.com:8080/path/?key=value,foo=bar#fragment" ));

    std::stringstream sstr;
    sstr << uri;
    BOOST_CHECK_EQUAL( sstr.str(), uriStr );

    sstr.str( "" );
    sstr << servus::URI( "http://www.example.com/path" );
    BOOST_CHECK_EQUAL( sstr.str(), "http://www.example.com/path" );

    sstr.str( "" );
    sstr << servus::URI( "/path" );
    BOOST_CHECK_EQUAL( sstr.str(), "/path" );
}

BOOST_AUTO_TEST_CASE(test_invalid_uri)
{
    BOOST_CHECK_THROW( servus::URI uri( "bad_schema:" ),
                       std::exception );
    BOOST_CHECK_THROW( servus::URI uri( "8ad-schema:" ),
                       std::exception );
    BOOST_CHECK_NO_THROW( servus::URI uri( "g00d-sch+ma:" ));
    BOOST_CHECK_THROW( servus::URI uri( "http://host:port" ),
                       std::exception );
    BOOST_CHECK_THROW( servus::URI uri( "http://host:" ),
                       std::exception );
}

BOOST_AUTO_TEST_CASE(test_corner_cases)
{
    servus::URI uri1( "path/foo:bar" );
    BOOST_CHECK_EQUAL( uri1.getPath(), "path/foo:bar" );
    servus::URI uri2( "//path/foo:bar" );
    BOOST_CHECK_EQUAL( uri2.getPath(), "//path/foo:bar" );
    servus::URI uri3( "?/##" );
    BOOST_CHECK_EQUAL( uri3.getQuery(), "/" );
    BOOST_CHECK_EQUAL( uri3.getFragment(), "#" );
    servus::URI uri4( "#:" );
    BOOST_CHECK_EQUAL( uri4.getFragment(), ":" );
    servus::URI uri5( "/foo#?" );
    BOOST_CHECK_EQUAL( uri5.getPath(), "/foo" );
    BOOST_CHECK_EQUAL( uri5.getFragment(), "?" );
    servus::URI uri6( "foo://*:0" );
    BOOST_CHECK_EQUAL( uri6.getScheme(), "foo" );
    BOOST_CHECK_EQUAL( uri6.getHost(), "*" );
}
