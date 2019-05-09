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
#include <QList>
#include "listing.h"
#include "filter.h"
#include <QDir>
#include <QSettings>

namespace Main_ {
static const QString DefaultXML = QDir::currentPath() + "/saved.xml";
}

/**
 * @brief The Main class
 */
class Main : public QObject {
    Q_OBJECT

public:
    static Main *instance() { static Main *instance( new Main()); return instance; }
    ~Main();
    QList<Listing> listings;
    QList<Filter> filters;

public slots:
    void storeListings( const QString &path = Main_::DefaultXML );
    void readListings( const QString &path = Main_::DefaultXML );

private:
    explicit Main() {}
};
