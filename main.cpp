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
#include "mainwindow.h"
#include "main.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QTranslator>
//#include <QDebug>

//
// defines
//
#define FORCE_LATVIAN

/**
 * @brief qMain
 * @param argc
 * @param argv
 * @return
 */
int main( int argc, char *argv[] ) {
    QApplication app( argc, argv );

    // set up settings
    Main::instance()->settings = new QSettings( "Factory12", "FindAFlat" );

    // set up translator
    QTranslator translator;
    QString locale;
#ifndef FORCE_LATVIAN
    locale = QLocale::system().name();
#else
    locale = "lv_LV";
#endif
    translator.load( ":/i18n/" + locale );
    app.installTranslator( &translator );

    // set up gui
    MainWindow gui;
    gui.show();
    QLoggingCategory::setFilterRules( "qt.network.ssl.warning=false" );

    // clean up
    QObject::connect( qApp, &QApplication::aboutToQuit, []() {
        delete Main::instance();
    } );

    // exec
    return app.exec();
}
