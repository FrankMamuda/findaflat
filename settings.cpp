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
#include "settings.h"
#include "main.h"
#include "mainwindow.h"

/**
 * @brief Settings::~Settings
 */
Settings::~Settings() {
    // store settings
    this->setValue( "filter/minPrice", MainWindow::instance()->priceMin());
    this->setValue( "filter/maxPrice", MainWindow::instance()->priceMax());
    this->setValue( "filter/minArea", MainWindow::instance()->areaMin());
    this->setValue( "filter/maxArea", MainWindow::instance()->areaMax());
    this->setValue( "filter/minFloor", MainWindow::instance()->floorMin());
    this->setValue( "filter/minRooms", MainWindow::instance()->roomsMin());
    this->setValue( "filter/maxRooms", MainWindow::instance()->roomsMax());
    this->setValue( "filter/url", MainWindow::instance()->url());
    this->setValue( "notifications/enabled", MainWindow::instance()->notificationsEnabled());
    this->setValue( "notifications/token", MainWindow::instance()->token());
    this->setValue( "notifications/topic", MainWindow::instance()->topic());

    // store filters
    int y = 0;
    foreach ( const Filter &filter, Main::instance()->filters )
        Settings::instance()->setValue( QString( "filter/filter_%1" ).arg( y++ ), filter.settingsString());

    Settings::instance()->setValue( "filter/numFilters",  Main::instance()->filters.count());
}
