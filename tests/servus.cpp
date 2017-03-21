/* Copyright (c) 2012-2016, Stefan.Eilemann@epfl.ch
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

#include <random>

#ifdef SERVUS_USE_DNSSD
#include <dns_sd.h>
#endif

#ifdef _MSC_VER
#include <windows.h>
#define _sleep Sleep
#else
#define _sleep ::sleep
#endif

static const int _propagationTime = 1000;
static const int _propagationTries = 10;

uint16_t getRandomPort()
{
    static std::random_device device;
    static std::minstd_rand engine(device());
    std::uniform_int_distribution<uint16_t> generator(1024u, 65535u);
    return generator(engine);
}

BOOST_AUTO_TEST_CASE(test_servus)
{
    const uint32_t port = getRandomPort();
    std::string serviceName = "_servustest_" + std::to_string(port) + "._tcp";

    try
    {
        servus::Servus service(serviceName);
    }
    catch (const std::runtime_error& e)
    {
        if (getenv("TRAVIS"))
        {
            std::cerr << "Bailing, no avahi on a Travis CI setup" << std::endl;
            BOOST_CHECK_EQUAL(e.what(),
                              "Can't setup avahi client: Daemon not running");
            return;
        }
        throw;
    }

    servus::Servus service(serviceName);
    const servus::Servus::Result& result =
        service.announce(port, std::to_string(port));

    BOOST_CHECK_EQUAL(service.getName(), serviceName);

    if (!servus::Servus::isAvailable())
    {
        // BOOST_CHECK_EQUAL gives a link error for Result::NOT_SUPPORTED
        BOOST_CHECK(result == servus::Servus::Result::NOT_SUPPORTED);
        return;
    }

#ifdef SERVUS_USE_DNSSD
    BOOST_CHECK(servus::Result::SUCCESS == kDNSServiceErr_NoError);
#endif
    if (result != servus::Result::SUCCESS) // happens on CI VMs
    {
        std::cerr << "Bailing, got " << result
                  << ": looks like a broken zeroconf setup" << std::endl;
        return;
    }
    BOOST_CHECK_EQUAL(result, result);

    service.withdraw();
    service.set("foo", "bar");
    BOOST_CHECK_EQUAL(service.get("foo"), "bar");
    BOOST_CHECK_EQUAL(service.get("bar"), std::string());
    BOOST_CHECK(service.announce(port, std::to_string(port)));

    int nLoops = _propagationTries;
    while (--nLoops)
    {
        const servus::Strings& hosts =
            service.discover(servus::Servus::IF_LOCAL, _propagationTime);
        if (hosts.empty() && nLoops > 1)
        {
            if (getenv("TRAVIS"))
            {
                std::cerr << "Bailing, got no hosts on a Travis CI setup"
                          << std::endl;
                return;
            }
            continue;
        }

        BOOST_REQUIRE_EQUAL(hosts.size(), 1);
        BOOST_CHECK_EQUAL(hosts.front(), std::to_string(port));
        BOOST_CHECK(service.containsKey(hosts.front(), "foo"));
        BOOST_CHECK_EQUAL(service.get(hosts.front(), "foo"), "bar");
        BOOST_CHECK_EQUAL(service.get("bar", "foo"), std::string());
        BOOST_CHECK_EQUAL(service.get(hosts.front(), "foobar"), std::string());
        break;
    }

    service.set("foobar", "42");

    nLoops = _propagationTries;
    while (--nLoops)
    {
        const servus::Strings& hosts =
            service.discover(servus::Servus::IF_LOCAL, _propagationTime);
        const bool hasFoobar =
            !hosts.empty() && service.containsKey(hosts.front(), "foobar");
        if ((hosts.empty() || !hasFoobar) && nLoops > 1)
            continue;

        BOOST_REQUIRE_EQUAL(hosts.size(), 1);
        BOOST_CHECK_EQUAL(service.get(hosts.front(), "foobar"), "42");
        BOOST_CHECK_EQUAL(service.getKeys().size(), 2);
        break;
    }

    // continuous browse API
    BOOST_CHECK(!service.isBrowsing());
    BOOST_CHECK(service.beginBrowsing(servus::Servus::IF_LOCAL));
    BOOST_CHECK(service.isBrowsing());
    // BOOST_CHECK_EQUAL gives a link error for Result::PENDING
    BOOST_CHECK(service.beginBrowsing(servus::Servus::IF_LOCAL) ==
                servus::Servus::Result::PENDING);
    BOOST_CHECK(service.isBrowsing());

    BOOST_CHECK_EQUAL(service.browse(_propagationTime), service.browse(0));
    servus::Strings hosts = service.getInstances();
    BOOST_REQUIRE_EQUAL(hosts.size(), 1);
    BOOST_CHECK_EQUAL(service.get(hosts.front(), "foo"), "bar");
    BOOST_CHECK_EQUAL(service.getKeys().size(), 2);

    { // test updates during browsing
        servus::Servus service2(serviceName);
        BOOST_CHECK(service2.announce(port + 1, std::to_string(port + 1)));

        nLoops = _propagationTries;
        while (--nLoops)
        {
            BOOST_CHECK(service.browse(_propagationTime));
            hosts = service.getInstances();
            if (hosts.size() < 2 && nLoops > 1)
                continue;

            BOOST_CHECK_EQUAL(hosts.size(), 2);
            break;
        }
    }

    nLoops = _propagationTries;
    while (--nLoops)
    {
        BOOST_CHECK(service.browse(_propagationTime));
        hosts = service.getInstances();
        if (hosts.size() > 1 && nLoops > 1)
            continue;

        BOOST_CHECK_EQUAL(hosts.size(), 1);
        break;
    }

    BOOST_CHECK(service.isBrowsing());
    service.endBrowsing();
    BOOST_CHECK(!service.isBrowsing());

    hosts = service.getInstances();
    BOOST_REQUIRE_EQUAL(hosts.size(), 1);
    BOOST_CHECK_EQUAL(service.get(hosts.front(), "foo"), "bar");
    BOOST_CHECK_EQUAL(service.getKeys().size(), 2);
}
