/*
 * Copyright (C) 2016-2019 Factory #12
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

//
// includes
//
#include "filtermodel.h"

/**
 * @brief FilterModel::data
 * @param index
 * @param role
 * @return
 */
QVariant FilterModel::data( const QModelIndex &index, int role ) const {
    if ( !index.isValid())
        return QVariant();

    if ( index.row() >= Main::instance()->filterList.count())
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        const Filter *filter( Main::instance()->filterList.at( index.row()));

        return this->tr( "Rooms %1-%2, floor-%3, area %4-%5, price EUR %6-%7" )
                .arg( filter->roomsMin()).arg( filter->roomsMax())
                .arg( filter->floorMin())
                .arg( filter->areaMin()).arg( filter->areaMax())
                .arg( filter->priceMin()).arg( filter->priceMax());
    }

    return QVariant();
}
