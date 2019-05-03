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
#include "listing.h"
#include <QStringList>
#include <QDebug>

/**
 * @brief Listing::readData
 * @param input
 * @param pattern
 * @return
 */
QVariant Listing::readData( const QString &data, const QString &pattern ) const {
    QStringList list;
    const QRegExp rx( pattern );
    int pos = 0;

    while (( pos = rx.indexIn( data, pos )) != -1 ) {
        list << rx.cap( 1 );
        pos += rx.matchedLength();
    }

    if ( !list.isEmpty())
        return list.first();

    return QVariant();
}

/**
 * @brief Listing::parseRawXML
 * @param data
 */
void Listing::parseRawXML( const QString &data ) {
    // simplify xml
    const QString simplified( QString( data ).replace( ",", "" ).replace( "<b>", "" ).replace( "<br>", "" ).replace( "</b>", "<" ).replace( "</br>", "" ));

    // read data
    this->setRooms( this->readData( simplified, "Ist.:\\s+(\\d+)<" ).toInt());
    this->setArea( this->readData( simplified, "m2:\\s+(\\d+)<" ).toInt());
    this->setFloor( this->readData( simplified, "Stāvs:\\s+(\\d+).(\\d+)<" ).toInt());
    this->setAddress( this->readData( simplified, "Iela:\\s+([^<]+)<" ).toString());
    this->setPrice( this->readData( simplified, "Cena:\\s+(\\d+)" ).toInt());
}
