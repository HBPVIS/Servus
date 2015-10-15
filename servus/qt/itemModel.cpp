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

#include "itemModel.h"

#include <servus/listener.h>
#include <servus/servus.h>

#include <QTimer>

namespace servus
{
namespace qt
{

class ItemModel::Impl : public Listener
{
public:
    Impl( Servus& service_, ItemModel& parent_ )
        : parent( parent_ )
        , service( service_ )
        , rootItem( new QObject( ))
    {
        rootItem->setObjectName( QString( "Instances for %1" )
                            .arg( QString::fromStdString( service.getName( ))));

        for( const std::string& i : service.getInstances( ))
            _addInstanceItem( QString::fromStdString( i ));

        service.addListener( this );
        service.beginBrowsing( Servus::IF_ALL );

        browseTimer.connect( &browseTimer, &QTimer::timeout,
                             [this]() { service.browse( 0 ); } );
        browseTimer.start( 100 );
    }

    ~Impl()
    {
        browseTimer.stop();
        service.removeListener( this );
        service.endBrowsing();
    }

    void instanceAdded( const std::string& instance ) final
    {
        const QString& qstr = QString::fromStdString( instance );
        if( rootItem->findChild< QObject* >( qstr ))
            return;

        parent.beginInsertRows( QModelIndex(), rootItem->children().size(),
                                               rootItem->children().size( ));
        _addInstanceItem( qstr );
        parent.endInsertRows();
    }

    void instanceRemoved( const std::string& instance ) final
    {
        const QString& qstr = QString::fromStdString( instance );
        QObject* child = rootItem->findChild< QObject* >( qstr );
        if( !child )
            return;

        const QObjectList& children = rootItem->children();
        const int childIdx = children.indexOf( child );
        parent.beginRemoveRows( QModelIndex(), childIdx, childIdx );
        _removeInstanceItem( qstr );
        parent.endRemoveRows();
    }

    ItemModel& parent;
    Servus& service;
    std::unique_ptr< QObject > rootItem;
    QTimer browseTimer;

private:
    void _addInstanceItem( const QString& instance )
    {
        const std::string& instanceStr = instance.toStdString();
        QObject* instanceItem = new QObject( rootItem.get( ));
        instanceItem->setObjectName( instance );
        const Strings& keys = service.getKeys( instanceStr );
        for( const std::string& key : keys )
        {
            const QString data = QString( "%1 = %2" )
                .arg( QString::fromStdString( key ))
                .arg( QString::fromStdString( service.get( instanceStr, key )));
            QObject* kvItem = new QObject( instanceItem );
            kvItem->setObjectName( data );
        }
    }

    void _removeInstanceItem( const QString& instance )
    {
        QObject* child = rootItem->findChild< QObject* >( instance );
        delete child;
    }
};

ItemModel::ItemModel( Servus& service, QObject* parent_ )
    : QAbstractItemModel( parent_ )
    , _impl( new Impl( service, *this ))
{
}

ItemModel::~ItemModel()
{
}

QModelIndex ItemModel::index( const int row, const int column,
                              const QModelIndex& parent_ ) const
{
    if( !hasIndex( row, column, parent_ ))
        return QModelIndex();

    QObject* parentItem;
    if( !parent_.isValid( ))
        parentItem = _impl->rootItem.get();
    else
        parentItem = static_cast< QObject* >( parent_.internalPointer( ));

    QObject* childItem = parentItem->children()[row];
    if( childItem )
        return createIndex( row, column, childItem );
    return QModelIndex();
}

QModelIndex ItemModel::parent( const QModelIndex& index_ ) const
{
    if( !index_.isValid( ))
        return QModelIndex();

    QObject* childItem = static_cast< QObject* >( index_.internalPointer( ));
    QObject* parentItem = childItem->parent();

    if( parentItem == _impl->rootItem.get( ))
        return QModelIndex();

    if( !parentItem->parent( ))
        return createIndex( 0, 0, parentItem );
    const int row = parentItem->parent()->children().indexOf( parentItem );
    return createIndex( row, 0, parentItem );
}

int ItemModel::rowCount( const QModelIndex& parent_ ) const
{
    QObject* parentItem;
    if( !parent_.isValid( ))
        parentItem = _impl->rootItem.get();
    else
        parentItem = static_cast< QObject* >( parent_.internalPointer( ));

    return parentItem->children().size();
}

int ItemModel::columnCount( const QModelIndex& index_ ) const
{
    Q_UNUSED( index_ );
    return 1;
}

QVariant ItemModel::data( const QModelIndex& index_, const int role ) const
{
    if( !index_.isValid( ))
        return QVariant();

    QObject* item = static_cast< QObject* >( index_.internalPointer( ));

    switch( role )
    {
    case Qt::DisplayRole:
        return item->objectName();
    default:
        return QVariant();
    }
}

QVariant ItemModel::headerData( const int section,
                                const Qt::Orientation orientation,
                                const int role ) const
{
    Q_UNUSED( section );
    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return _impl->rootItem->objectName();

    return QVariant();
}

}
}
