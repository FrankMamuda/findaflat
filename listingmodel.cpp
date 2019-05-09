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

//
// includes
//
#include "listingmodel.h"
#include "main.h"

/**
 * @brief ListingModel::rowCount
 * @return
 */
int ListingModel::rowCount( const QModelIndex & ) const {
    return Main::instance()->listings.count();
}

/**
 * @brief ListingModel::data
 * @param index
 * @param role
 * @return
 */
QVariant ListingModel::data( const QModelIndex &index, int role ) const {
    if ( !index.isValid())
        return QVariant();

    if ( index.row() >= Main::instance()->listings.count())
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        const Listing &listing( Main::instance()->listings.at( index.row()));

        switch ( index.column()) {
        case Address:
            return listing.address();

        case Rooms:
            return listing.rooms();

        case Area:
            return listing.area();

        case Floor:
            return listing.floor();

        case Price:
            return listing.price();

        case Date:
            return listing.dateTime().toString( "yyyy.MM.dd" );

        case Time:
            return listing.dateTime().toString( "hh:mm" );
        }
    } else if ( role == Qt::TextAlignmentRole ) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

/**
 * @brief ListingModel::headerData
 * @param section
 * @param orientation
 * @param role
 * @return
 */
QVariant ListingModel::headerData( int section, Qt::Orientation orientation, int role ) const {
    // define column headers here for now (required for proper translations)
   /* this->columnHeaders <<
                           this->tr( "Address" ) <<
                           this->tr( "Rooms" ) <<
                           this->tr( "Area" ) <<
                           this->tr( "Floor" ) <<
                           this->tr( "Date" ) <<
                           this->tr( "Price" );*/

    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
        case Address:
            return this->tr( "Address" );

        case Rooms:
            return this->tr( "Rooms" );

        case Area:
            return this->tr( "Area" );

        case Floor:
            return this->tr( "Floor" );

        case Price:
            return this->tr( "Price" );

        case Date:
            return this->tr( "Date" );

        case Time:
            return this->tr( "Time" );
        }
    }

    return QAbstractTableModel::headerData( section, orientation, role );
}
