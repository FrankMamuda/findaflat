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

#ifndef FLAT_H
#define FLAT_H

//
// includes
//
#include <QObject>
#include <QDateTime>

/**
 * @brief The Flat class
 */
class Flat : public QObject {
    Q_OBJECT

public:
    explicit Flat( QObject *parent = 0 ) : QObject( parent ), m_rooms( 0 ), m_price( 0 ), m_area( 0 ), m_floor( 0 ), m_floor_total( 0 ) { }
    int rooms() const { return this->m_rooms; }
    int price() const { return this->m_price; }
    int pricePerSquare() const { return this->m_price / this->m_area; }
    int area() const { return this->m_area; }
    int floor() const { return this->m_floor; }
    int totalFloors() const { return this->m_floor_total; }
    QString description() const { return this->m_description; }
    QString link() const { return this->m_link; }
    QString address() const { return this->m_address; }
    QDateTime dateTime() const { return this->m_dateTime; }

signals:

public slots:
    void setRooms( int rooms ) { this->m_rooms = rooms; }
    void setPrice( int price ) { this->m_price = price; }
    void setArea( int area ) { this->m_area = area; }
    void setFloor( int floor ) { this->m_floor = floor; }
    void setTotalFloors( int floor ) { this->m_floor_total = floor; }
    void setDescription( const QString &description ) { this->m_description = description; }
    void setLink( const QString &link ) { this->m_link = link; }
    void setAddress( const QString &address ) { this->m_address = address; }
    void setDateTime( const QDateTime &dateTime ) { this->m_dateTime = dateTime; }
    void parseRawXML( const QString &data );

private:
    int m_rooms;
    int m_price;
    int m_area;
    int m_floor;
    int m_floor_total;
    QString m_description;
    QString m_link;
    QString m_address;
    QDateTime m_dateTime;
    QVariant readData( const QString &data, const QString &pattern ) const;
};

#endif // FLAT_H
