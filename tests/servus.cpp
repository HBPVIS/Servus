/* Copyright (c) 2012-2015, Stefan.Eilemann@epfl.ch
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

#define BOOST_TEST_MODULE servus_servus
#include <boost/test/unit_test.hpp>

#include <servus/servus.h>

#ifdef COMMON_USE_CXX03
#  include <boost/random.hpp>
#  include <boost/lexical_cast.hpp>
    namespace rnd=boost::random;
#else
#  include <random>
    namespace rnd=std;
#endif

#ifdef SERVUS_USE_DNSSD
#  include <dns_sd.h>
#endif

#ifdef _MSC_VER
#  define sleep Sleep
#endif

uint16_t getRandomPort()
{
#ifdef COMMON_USE_CXX03
    static rnd::minstd_rand engine;
#else
    static rnd::random_device device;
    static rnd::minstd_rand engine( device( ));
#endif
    rnd::uniform_int_distribution< uint16_t > generator( 1024u, 65535u );
    return generator( engine );
}

template< class T > std::string toString( const T& what )
{
#ifdef COMMON_USE_CXX03
    return boost::lexical_cast< std::string >( what );
#else
    return std::to_string( what );
#endif
}

BOOST_AUTO_TEST_CASE(test_servus)
{
    const uint32_t port = getRandomPort();
    std::string serviceName = "_servustest_" + toString( port ) + "._tcp";

    try
    {
        servus::Servus service( serviceName );
    }
    catch( const std::runtime_error& e )
    {
        if( getenv( "TRAVIS" ))
        {
            std::cerr << "Bailing, no avahi on a Travis CI setup" << std::endl;
            BOOST_CHECK_EQUAL(
                e.what(), "Can't setup avahi client: Daemon not running" );
            return;
        }
        throw;
    }

    servus::Servus service( serviceName );
    const servus::Servus::Result& result = service.announce( port,
                                                             toString( port ));

    BOOST_CHECK_EQUAL( service.getName(), serviceName );

    if( !servus::Servus::isAvailable( ))
    {
        // BOOST_CHECK_EQUAL gives a link error for Result::NOT_SUPPORTED
        BOOST_CHECK( result == servus::Servus::Result::NOT_SUPPORTED );
        return;
    }

#ifdef SERVUS_USE_DNSSD
    BOOST_CHECK( servus::Result::SUCCESS == kDNSServiceErr_NoError );
#endif
    if( result != servus::Result::SUCCESS ) // happens on CI VMs
    {
        std::cerr << "Bailing, got " << result
                  << ": looks like a broken zeroconf setup" << std::endl;
        return;
    }
    BOOST_CHECK_EQUAL( result, result );

    service.withdraw();
    service.set( "foo", "bar" );
    BOOST_CHECK_EQUAL( service.get( "foo" ), "bar" );
    BOOST_CHECK_EQUAL( service.get( "bar" ), std::string( ));
    BOOST_CHECK( service.announce( port, toString( port )));

    servus::Strings hosts = service.discover( servus::Servus::IF_LOCAL, 2000 );
    if( hosts.empty() && getenv( "TRAVIS" ))
    {
        std::cerr << "Bailing, got no hosts on a Travis CI setup" << std::endl;
        return;
    }

    BOOST_REQUIRE_EQUAL( hosts.size(), 1 );
    BOOST_CHECK_EQUAL( hosts.front(), toString( port ));
    BOOST_CHECK( service.containsKey( hosts.front(), "foo" ));
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foo" ), "bar" );
    BOOST_CHECK_EQUAL( service.get( "bar", "foo" ), std::string( ));
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foobar" ), std::string( ));
    ::sleep( 1 );

    service.set( "foobar", "42" );

    ::sleep( 2 );

    hosts = service.discover( servus::Servus::IF_LOCAL, 2000 );
    BOOST_REQUIRE_EQUAL( hosts.size(), 1 );
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foobar" ), "42" );
    BOOST_CHECK_EQUAL( service.getKeys().size(), 2 );

    // continuous browse API
    BOOST_CHECK( !service.isBrowsing( ));
    BOOST_CHECK( service.beginBrowsing( servus::Servus::IF_LOCAL ));
    BOOST_CHECK( service.isBrowsing( ));
    // BOOST_CHECK_EQUAL gives a link error for Result::PENDING
    BOOST_CHECK( service.beginBrowsing( servus::Servus::IF_LOCAL ) ==
                 servus::Servus::Result::PENDING );
    BOOST_CHECK( service.isBrowsing( ));

    BOOST_CHECK_EQUAL( service.browse( 200 ), service.browse( 0 ));
    hosts = service.getInstances();
    BOOST_REQUIRE_EQUAL( hosts.size(), 1 );
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foo" ), "bar" );
    BOOST_CHECK_EQUAL( service.getKeys().size(), 2 );

    { // test updates during browsing
        servus::Servus service2( serviceName );
        BOOST_CHECK( service2.announce( port+1, toString( port+1 )));
        BOOST_CHECK( service.browse( 2000 ));
        hosts = service.getInstances();
        BOOST_CHECK_EQUAL( hosts.size(), 2 );
    }
    ::sleep( 1 );

    BOOST_CHECK( service.browse( 2000 ));
    hosts = service.getInstances();
    BOOST_CHECK_EQUAL( hosts.size(), 1 );

    BOOST_CHECK( service.isBrowsing( ));
    service.endBrowsing();
    BOOST_CHECK( !service.isBrowsing( ));

    hosts = service.getInstances();
    BOOST_REQUIRE_EQUAL( hosts.size(), 1 );
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foo" ), "bar" );
    BOOST_CHECK_EQUAL( service.getKeys().size(), 2 );
}
