/* Copyright (c) 2015, Human Brain Project
 *                     Daniel.Nachbaur@epfl.ch
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

#define BOOST_TEST_MODULE servus_itemModel
#include <boost/test/unit_test.hpp>

#include <atomic>
#include <chrono>

#include <QApplication>

#include <servus/listener.h>
#include <servus/qt/itemModel.h>
#include <servus/servus.h>
#include <servus/uint128_t.h>

const std::string TEST_INSTANCE = "testInstance";
const size_t DISCOVER_TIMEOUT = 15 /*seconds*/;

struct GlobalQtApp
{
    GlobalQtApp()
        : app()
    {
        boost::unit_test::master_test_suite_t& testSuite =
            boost::unit_test::framework::master_test_suite();
        app.reset(new QCoreApplication(testSuite.argc, testSuite.argv));
    }

    std::unique_ptr<QCoreApplication> app;
};

class Watchdog
{
public:
    Watchdog()
#ifdef __APPLE__
        : gotUpdate(false)
#else
        : gotUpdate(ATOMIC_VAR_INIT(false))
#endif
    {
    }

    void wait()
    {
        QCoreApplication* app = QCoreApplication::instance();
        const auto startTime = std::chrono::high_resolution_clock::now();
        while (!gotUpdate)
        {
            app->processEvents(QEventLoop::AllEvents, 100 /*ms*/);
            const auto duration =
                std::chrono::high_resolution_clock::now() - startTime;
            const size_t elapsed =
                std::chrono::nanoseconds(duration).count() / 1000000000;
            if (elapsed > DISCOVER_TIMEOUT)
                return;
        }
    }

    std::atomic_bool gotUpdate;
};

class WatchAdd : public Watchdog, public servus::Listener
{
public:
    WatchAdd()
        : Watchdog()
    {
    }

    void instanceAdded(const std::string& instance) final
    {
        BOOST_CHECK_EQUAL(instance, TEST_INSTANCE);
        gotUpdate = true;
    }

    void instanceRemoved(const std::string&) final {}
};

class WatchRemove : public Watchdog, public servus::Listener
{
public:
    WatchRemove()
        : Watchdog()
    {
    }

    void instanceAdded(const std::string& instance) final
    {
        BOOST_CHECK_EQUAL(instance, TEST_INSTANCE);
    }

    void instanceRemoved(const std::string& instance) final
    {
        BOOST_CHECK_EQUAL(instance, TEST_INSTANCE);
        gotUpdate = true;
    }
};

BOOST_GLOBAL_FIXTURE(GlobalQtApp);

BOOST_AUTO_TEST_CASE(invalidAccess)
{
    servus::Servus service(servus::TEST_DRIVER);
    const servus::qt::ItemModel model(service);

    const QVariant invalidHeader =
        model.headerData(0, Qt::Vertical, Qt::DisplayRole);
    BOOST_CHECK(invalidHeader == QVariant());

    const QModelIndex invalidIndex = model.index(1, 1);
    BOOST_CHECK(invalidIndex == QModelIndex());
    BOOST_CHECK(QModelIndex() == model.parent(invalidIndex));
}

BOOST_AUTO_TEST_CASE(servusItemModel)
{
    servus::Servus service(servus::TEST_DRIVER);
    const servus::qt::ItemModel model(service);

    WatchAdd watchAdd;
    service.addListener(&watchAdd);

    BOOST_CHECK_EQUAL(model.rowCount(), 0);
    BOOST_CHECK_EQUAL(model.columnCount(), 1);
    const std::string header =
        model.headerData(0, Qt::Horizontal, Qt::DisplayRole)
            .toString()
            .toStdString();
    BOOST_CHECK_EQUAL(header, "Instances for " + servus::TEST_DRIVER);
    BOOST_CHECK(model.data(QModelIndex()) == QVariant());

    servus::Servus service2(servus::TEST_DRIVER);
    const servus::Servus::Result& result = service2.announce(0, TEST_INSTANCE);
    if (result != servus::Result::SUCCESS) // happens on CI VMs
    {
        std::cerr << "Bailing, got " << result
                  << ": looks like a broken zeroconf setup" << std::endl;
        return;
    }

    service2.set("foo", "bar");

    watchAdd.wait();
    service.removeListener(&watchAdd);
    if (!watchAdd.gotUpdate && getenv("TRAVIS"))
    {
        std::cerr << "Bailing, got no hosts on a Travis CI setup" << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL(model.rowCount(), 1);
    BOOST_CHECK_EQUAL(model.columnCount(), 1);
    const QModelIndex instanceIndex = model.index(0, 0);
    BOOST_CHECK(model.parent(instanceIndex) == QModelIndex());
    BOOST_CHECK_EQUAL(model.data(instanceIndex).toString().toStdString(),
                      TEST_INSTANCE);
    BOOST_CHECK_EQUAL(
        model.data(instanceIndex, Qt::UserRole).toString().toStdString(),
        service.get(TEST_INSTANCE, "servus_host"));
    BOOST_CHECK(model.data(instanceIndex, Qt::EditRole) == QVariant());
    BOOST_REQUIRE_EQUAL(model.rowCount(instanceIndex), 3);
    const QModelIndex kv1Index = model.index(0, 0, instanceIndex);
    BOOST_CHECK(model.parent(kv1Index) == instanceIndex);
    BOOST_CHECK(model.data(kv1Index, Qt::UserRole) == QVariant());
    const QVariant kv1 = model.data(kv1Index);
    const QVariant kv2 = model.data(model.index(1, 0, instanceIndex));
    const QVariant kv3 = model.data(model.index(2, 0, instanceIndex));
    BOOST_REQUIRE_EQUAL(model.rowCount(kv1Index), 0);
    BOOST_CHECK_EQUAL(kv1.toString().toStdString(), "foo = bar");
    BOOST_CHECK(kv2.toString().startsWith("servus_host = "));
    BOOST_CHECK(kv3.toString().startsWith("servus_port = "));

    WatchRemove watchRemove;
    service.addListener(&watchRemove);

    service2.withdraw();

    watchRemove.wait();
    service.removeListener(&watchRemove);
    if (!watchRemove.gotUpdate && getenv("TRAVIS"))
    {
        std::cerr << "Bailing, no functioning discovery on a Travis CI setup"
                  << std::endl;
        return;
    }

    BOOST_CHECK_EQUAL(model.rowCount(), 0);
}
