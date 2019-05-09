/*
 * Copyright (C) 2019 Factory #12
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 */

#pragma once

//
// includes
//
#include <QAbstractTableModel>

/**
 * @brief The ListingModel class
 */
class ListingModel : public QAbstractTableModel {
public:
    enum Columns {
        Address = 0,
        Rooms,
        Area,
        Floor,
        Date,
        Time,
        Price,

        Count
    };

    explicit ListingModel( QObject *parent = nullptr ) : QAbstractTableModel( parent ) {}
    int rowCount( const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override { return Columns::Count; }
    QVariant data( const QModelIndex &, int ) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
    void beginReset() { this->beginResetModel(); }
    void endReset() { this->endResetModel(); }
};
