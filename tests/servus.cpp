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

#include "servus/servus.h"

#define BOOST_TEST_MODULE
#include <boost/test/unit_test.hpp>

#include <random>
#include <thread>

#ifdef SERVUS_USE_DNSSD
#  include <dns_sd.h>
#endif


uint16_t getRandomPort()
{
    static std::random_device device;
    static std::minstd_rand engine( device( ));
    std::uniform_int_distribution< uint16_t > generator( 1024, 65535u );
    return generator( engine );
}

BOOST_AUTO_TEST_CASE(test_servus)
{
    const uint32_t port = getRandomPort();
    std::string serviceName = "_servustest_" + std::to_string( port ) + "._tcp";

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
                                    std::to_string( port ));

    BOOST_CHECK_EQUAL( service.getName(), serviceName );

    if( !servus::Servus::isAvailable( ))
    {
        // BOOST_CHECK_EQUAL gives a link error for Result::NOT_SUPPORTED
        BOOST_CHECK( result == servus::Servus::Result::NOT_SUPPORTED );
        return;
    }

#ifdef SERVUS_USE_DNSSD
    BOOST_CHECK_EQUAL( servus::Result::SUCCESS == kDNSServiceErr_NoError );
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
    BOOST_CHECK( service.announce( port, std::to_string( port )));

    servus::Strings hosts = service.discover( servus::Servus::IF_LOCAL, 2000 );
    if( hosts.empty() && getenv( "TRAVIS" ))
    {
        std::cerr << "Bailing, got no hosts on a Travis CI setup" << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL( hosts.size(), 1 );

    BOOST_CHECK_EQUAL( hosts.front(), std::to_string( port ));
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foo" ), "bar" );
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ));

    service.set( "foobar", "42" );
    std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ));

    hosts = service.discover( servus::Servus::IF_LOCAL, 2000 );
    BOOST_CHECK_EQUAL( hosts.size(), 1 );
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
    BOOST_CHECK_EQUAL( hosts.size(), 1 );
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foo" ), "bar" );
    BOOST_CHECK_EQUAL( service.getKeys().size(), 2 );

    { // test updates during browsing
        servus::Servus service2( serviceName );
        BOOST_CHECK( service2.announce( port+1, std::to_string( port+1 )));
        BOOST_CHECK( service.browse( 2000 ));
        hosts = service.getInstances();
        BOOST_CHECK_EQUAL( hosts.size(), 2 );
    }
    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ));

    BOOST_CHECK( service.browse( 2000 ));
    hosts = service.getInstances();
    BOOST_CHECK_EQUAL( hosts.size(), 1 );

    BOOST_CHECK( service.isBrowsing( ));
    service.endBrowsing();
    BOOST_CHECK( !service.isBrowsing( ));

    hosts = service.getInstances();
    BOOST_CHECK_EQUAL( hosts.size(), 1 );
    BOOST_CHECK_EQUAL( service.get( hosts.front(), "foo" ), "bar" );
    BOOST_CHECK_EQUAL( service.getKeys().size(), 2 );
}
