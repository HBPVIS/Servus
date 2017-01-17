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

#include <servus/servus.h>
#include <servus/version.h>
#include <servus/qt/itemModel.h>

#include <QApplication>
#include <QFormLayout>
#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QTreeView>

int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    QGuiApplication::setApplicationVersion(
                QString::fromStdString( servus::Version::getString( )));
    app.setQuitOnLastWindowClosed( true );
    app.setApplicationName( "Servus Browser" );

    std::unique_ptr< servus::Servus > service;
    std::unique_ptr< servus::qt::ItemModel > model;

    QMainWindow window;
    QWidget* widget = new QWidget( &window );
    window.setCentralWidget( widget );

    QFormLayout* layout = new QFormLayout( widget );

    QLineEdit* lineEdit = new QLineEdit( widget );
    lineEdit->setText( "_zeroeq_pub._tcp" );
    layout->addRow( "Service name", lineEdit );

    QTreeView* view = new QTreeView( widget );
    view->setHeaderHidden( true );
    layout->addRow( view );
    widget->setLayout( layout );

    const auto onServiceChanged = [&]()
    {
        const std::string& serviceName = lineEdit->text().toStdString();
        if( service && service->getName() == serviceName )
            return;

        view->setModel( nullptr );
        model.reset();

        service.reset( new servus::Servus( serviceName ));
        model.reset( new servus::qt::ItemModel( *service ));
        view->setModel( model.get( ));
    };

    lineEdit->connect( lineEdit, &QLineEdit::returnPressed, onServiceChanged );
    onServiceChanged();

    window.resize( 400, 300 );
    window.show();
    return app.exec();
}
