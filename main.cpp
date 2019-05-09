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
#include "settings.h"

//
// defines
//
#define FORCE_LATVIAN

/**
 * @brief Main::~Main
 */
Main::~Main() {
    this->storeListings();
    this->filters.clear();
    this->listings.clear();
}

/**
 * @brief Main::storeListings
 * @param path
 */
void Main::storeListings( const QString &path ) {
    QFile xmlFile( path );

    if ( xmlFile.open( QFile::ReadWrite | QFile::Truncate )) {
        xmlFile.write( "<listings version=\"1.0\">\n" );

        foreach ( const Listing &listing, this->listings ) {
            xmlFile.write( QString( "<listing rooms=\"%1\" price=\"%2\" area=\"%3\" floor=\"%4\" total=\"%5\" description=\"%6\" link=\"%7\" address=\"%8\" date=\"%9\" />\n" )
                           .arg( listing.rooms())
                           .arg( listing.price())
                           .arg( listing.area())
                           .arg( listing.floor())
                           .arg( listing.totalFloors())
                           .arg( listing.description())
                           .arg( listing.link())
                           .arg( listing.address())
                           .arg( listing.dateTime().toString( "dd.MM.yyyy hh:mm" )).toUtf8()
                           );
        }

        xmlFile.write( "</listings>\n" );
        xmlFile.close();
    }
}

/**
 * @brief Main::readListings
 * @param path
 */
void Main::readListings( const QString &path ) {
    QFile xmlFile( path );

    this->listings.clear();

    if ( xmlFile.open( QFile::ReadOnly )) {
        QXmlStreamReader xml;
        xml.addData( xmlFile.readAll());

        if ( xml.readNextStartElement()) {
            if ( xml.name() == "listings" && xml.attributes().value( "version" ) == "1.0" ) {
                while ( xml.readNextStartElement()) {
                    if ( xml.name() == "listing" ) {
                        Listing listing;

                        listing.setRooms( xml.attributes().value( "rooms" ).toInt());
                        listing.setPrice( xml.attributes().value( "price" ).toInt());
                        listing.setArea( xml.attributes().value( "area" ).toInt());
                        listing.setFloor( xml.attributes().value( "floor" ).toInt());
                        listing.setTotalFloors( xml.attributes().value( "total" ).toInt());
                        listing.setDescription( xml.attributes().value( "description" ).toString());
                        listing.setLink( xml.attributes().value( "link" ).toString());
                        listing.setAddress( xml.attributes().value( "address" ).toString());
                        listing.setDateTime( QDateTime::fromString( xml.attributes().value( "date" ).toString(), "dd.MM.yyyy hh:mm" ));

                        // check for duplicates
                        bool found = false;
                        foreach ( const Listing &l, this->listings ) {
                            if ( !QString::compare( l.link(), listing.link()))
                                found = true;
                        }

                        if ( !found )
                            this->listings << listing;

                        xml.readNext();
                    } else
                        xml.skipCurrentElement();
                }
            } else
                xml.raiseError( this->tr( "invalid file or version" ));
        }

        // close file
        xmlFile.close();

        // fill listings
        MainWindow::instance()->fillListings();
    }
}

/**
 * @brief qMain
 * @param argc
 * @param argv
 * @return
 */
int main( int argc, char *argv[] ) {
    QApplication app( argc, argv );

    // set up translator
    QTranslator translator;
#ifndef FORCE_LATVIAN
    const QString locale( QLocale::system().name());
#else
    const QString locale( "lv_LV" );
#endif
    if ( translator.load( ":/i18n/" + locale ))
        app.installTranslator( &translator );

    // set up gui
    MainWindow::instance()->show();
    MainWindow::instance()->setupFilters();

    // ignore ssl warnings
    QLoggingCategory::setFilterRules( "qt.network.ssl.warning=false" );

    // clean up on exit
    QObject::connect( qApp, &QApplication::aboutToQuit, []() {
        delete Settings::instance();
        delete Main::instance();
        delete MainWindow::instance();
    } );

    // exec
    return app.exec();
}
