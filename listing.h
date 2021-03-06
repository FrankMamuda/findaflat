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

#pragma once

//
// includes
//
#include <QObject>
#include <QDateTime>

/**
 * @brief The Listing class
 */
class Listing {

public:
    explicit Listing() {}
    ~Listing() = default;
    int rooms() const { return this->m_rooms; }
    int price() const { return this->m_price; }
    int pricePerSquare() const { return this->m_price / this->m_area; }
    int area() const { return this->m_area; }
    int floor() const { return this->m_floor; }
    int totalFloors() const { return this->m_floorTotal; }
    QString description() const { return this->m_description; }
    QString link() const { return this->m_link; }
    QString imageURL() const { return this->m_imageURL; }
    QString address() const { return this->m_address; }
    QDateTime dateTime() const { return this->m_dateTime; }
    void setRooms( int rooms ) { this->m_rooms = rooms; }
    void setPrice( int price ) { this->m_price = price; }
    void setImageURL( const QString &url ) { this->m_imageURL = url; }
    void setArea( int area ) { this->m_area = area; }
    void setFloor( int floor ) { this->m_floor = floor; }
    void setTotalFloors( int floor ) { this->m_floorTotal = floor; }
    void setDescription( const QString &description ) { this->m_description = description; }
    void setLink( const QString &link ) { this->m_link = link; }
    void setAddress( const QString &address ) { this->m_address = address; }
    void setDateTime( const QDateTime &dateTime ) { this->m_dateTime = dateTime; }
    void parseRawXML( const QString &data );

private:
    int m_rooms = 0;
    int m_price = 0;
    int m_area = 0;
    int m_floor = 0;
    int m_floorTotal = 0;
    QString m_description;
    QString m_link;
    QString m_address;
    QDateTime m_dateTime;
    QString m_imageURL;
    QVariant readData( const QString &data, const QString &pattern ) const;
};
