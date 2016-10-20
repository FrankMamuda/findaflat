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
#include "flat.h"
#include <QStringList>
#include <QDebug>

/**
 * @brief Flat::Flat
 * @param parent
 */
Flat::Flat( QObject *parent ) : QObject( parent ), m_rooms( 0 ), m_price( 0 ), m_area( 0 ), m_floor( 0 ), m_floor_total( 0 ) {
}

/**
 * @brief Flat::readData
 * @param input
 * @param pattern
 * @return
 */
QVariant Flat::readData( const QString &data, const QString &pattern ) const {
    QStringList list;
    QRegExp rx( pattern );
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
 * @brief Flat::parseRawXML
 * @param data
 */
void Flat::parseRawXML( const QString &data ) {
    QString simplified;

    // simplify xml
    simplified = data;
    simplified = simplified.replace( ",", "" ).replace( "<b>", "" ).replace( "<br>", "" ).replace( "</b>", "<" ).replace( "</br>", "" );

    // read data
    this->setRooms( this->readData( simplified, "Ist.:\\s+(\\d+)<" ).toInt());
    this->setArea( this->readData( simplified, "m2:\\s+(\\d+)<" ).toInt());
    this->setFloor( this->readData( simplified, "Stāvs:\\s+(\\d+).(\\d+)<" ).toInt());
    this->setAddress( this->readData( simplified, "Iela:\\s+([^<]+)<" ).toString());
    this->setPrice( this->readData( simplified, "Cena:\\s+(\\d+)" ).toInt());
}
