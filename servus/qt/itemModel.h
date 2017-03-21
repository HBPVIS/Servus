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

#ifndef SERVUSQT_ITEMMODEL
#define SERVUSQT_ITEMMODEL

#include <servus/qt/api.h>
#include <servus/types.h>

#include <QAbstractItemModel> // base class
#include <memory>             // std::unique_ptr

namespace servus
{
namespace qt
{
/**
 * An item model on top of a Servus service, to be used in a Qt item view.
 *
 * The model is represented as a hierarchy with two levels where the first level
 * containts one row per discovered instance of the service, and the second
 * level contains one row per announced key-value pair.
 *
 * The model itself sets the given Servus instance into the browsing state and
 * asynchronously browses for new and/or deleted instances every 100ms.
 *
 * @version 1.2
 */
class ItemModel : public QAbstractItemModel
{
public:
    /**
     * Construct a new model by filling it with the current discovered instances
     * and put the service into browsing state.
     *
     * @param service the mutable service instance that the model represents
     * @param parent optional parent for memory ownership
     * @version 1.2
     */
    SERVUSQT_API ItemModel(Servus& service, QObject* parent = nullptr);

    /** Destruct the model and reset the service back to non-browsing state. */
    SERVUSQT_API virtual ~ItemModel();

    /** Mandatory override of QAbstractItemModel::index. */
    SERVUSQT_API QModelIndex
        index(int row, int colum,
              const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Mandatory override of QAbstractItemModel::parent.
     *
     * If index points to a key-value item, the parent will be the corresponding
     * instance. If index points to an instance, the parent will be
     * QModelIndex().
     */
    SERVUSQT_API QModelIndex parent(const QModelIndex& index) const override;

    /**
     * Mandatory override of QAbstractItemModel::rowCount.
     *
     * If index points is QModelIndex(), the row count will be the number of
     * discovered instances. If index points to an instance item, the row count
     * will be the number of announced key-value items. If index points to a
     * key-value item, the row count will always be 0.
     */
    SERVUSQT_API
    int rowCount(const QModelIndex& index = QModelIndex()) const override;

    /**
     * Mandatory override of QAbstractItemModel::columnCount.
     *
     * Independent of index, the column count will always be 1.
     */
    SERVUSQT_API
    int columnCount(const QModelIndex& index = QModelIndex()) const override;

    /**
     * Mandatory override of QAbstractItemModel::data.
     *
     * If index points to an instance item, the returned data for
     * Qt::DisplayRole will be the instance name, and for Qt::ToolTipRole and
     * Qt::UserRole the data will be the hostname. If index points to a
     * key-value item, the returned data for Qt::DisplayRole will be a string in
     * the format "key = value". For any other index and/or role, the returned
     * data will be QVariant().
     */
    SERVUSQT_API QVariant data(const QModelIndex& index,
                               int role = Qt::DisplayRole) const override;

    /**
     * Optional override of QAbstractItemModel::headerData.
     *
     * If orientation is Qt::Horizontal and role is Qt::DisplayRole, the
     * returned data will be a string in the format
     * "Instances for <service-name>". For any other input, the returned data
     * will be QVariant().
     */
    SERVUSQT_API QVariant headerData(int section, Qt::Orientation orientation,
                                     int role) const override;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
}
}

#endif // SERVUSQT_ITEMMODEL
