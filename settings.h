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

#pragma once

//
// includes
//
#include <QSettings>

/**
 * @brief The Settings_ namespace
 */
namespace Settings_ {
class MainWindow;
constexpr const char* Organization = "Factory12";
constexpr const char* Application = "FindAFlat";
}

/**
 * @brief The Settings class
 */
class Settings : public QSettings {

public:
    static Settings *instance() { static Settings *instance( new Settings( Settings_::Organization, Settings_::Application )); return instance; }
    ~Settings();

private:
    explicit Settings( const QString &organization, const QString &application, QObject *parent = nullptr ) : QSettings( organization, application, parent ) {}
};
