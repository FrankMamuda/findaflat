/*
 * Copyright (C) 2013-2016 Avotu Briezhaudzetava
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

    if ( index.row() >= m.filterList.count())
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        Filter *filter = m.filterList.at( index.row());

        return QString( "Istabas %1-%2, stāvs-%3, platība %4-%5, cena EUR %6-%7" )
                .arg( filter->roomsMin()).arg( filter->roomsMax())
                .arg( filter->floorMin())
                .arg( filter->areaMin()).arg( filter->areaMax())
                .arg( filter->priceMin()).arg( filter->priceMax());
    }

    return QVariant();
}
