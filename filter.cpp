/*
 * Copyright (C) 2016 Avotu Briezhaudzetava
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
#include "filter.h"
#include <QStringList>
#include "mainwindow.h"

/**
 * @brief Filter::Filter
 */
Filter::Filter( int priceMin, int priceMax, int areaMin, int areaMax, int floorMin, int roomsMin, int roomsMax )
    : m_priceMin( priceMin ), m_priceMax( priceMax ), m_areaMin( areaMin ), m_areaMax( areaMax ), m_floorMin( floorMin ), m_roomsMin( roomsMin ), m_roomsMax( roomsMax )
{
}

/**
 * @brief Filter::compare
 * @param flat
 * @return
 */
bool Filter::compare( const Flat *flat ) const {
    if ( flat->floor() < this->floorMin() ||
         flat->area() < this->areaMin() ||
         flat->area() > this->areaMax() ||
         flat->price() < this->priceMin() ||
         flat->price() > this->priceMax() ||
         flat->rooms() < this->roomsMin() ||
         flat->rooms() > this->roomsMax())
        return false;

    return true;
}

/**
 * @brief Filter::settingsString
 * @return
 */
QString Filter::settingsString() const {
    return QString( "%1;%2;%3;%4;%5;%6;%7" )
            .arg( this->priceMin()).arg( this->priceMax())
            .arg( this->areaMin()).arg( this->areaMax())
            .arg( this->floorMin())
            .arg( this->roomsMin()).arg( this->roomsMax());
}

/**
 * @brief Filter::fromString
 * @return
 */
Filter *Filter::fromString( const QString &filter ) {
    QStringList list = filter.split( ";" );

    if ( list.count() == 7 )
        return new Filter( list.at( 0 ).toInt(), list.at( 1 ).toInt(), list.at( 2 ).toInt(), list.at( 3 ).toInt(), list.at( 4 ).toInt(), list.at( 5 ).toInt(), list.at( 6 ).toInt());

    return new Filter( Ui::DefaultMinPrice, Ui::DefaultMaxPrice, Ui::DefaultMinArea, Ui::DefaultMaxArea, Ui::DefaultMinFloor, Ui::DefaultMinRooms, Ui::DefaultMaxRooms );
}
