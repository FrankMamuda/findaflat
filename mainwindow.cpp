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
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopServices>
#include <QTimer>
#include <QSettings>
#include <QFileDialog>
#include <QXmlStreamReader>
#include "main.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "settings.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow ) {
    this->ui->setupUi( this );

    // set up ranking view
    this->ui->rssTableView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    this->ui->rssTableView->setSortingEnabled( true );

    // set up sorting & view model
    this->ui->rssTableView->verticalHeader()->hide();
    this->proxy->setSourceModel( this->listingModel );
    this->proxy->setDynamicSortFilter( true );
    this->ui->rssTableView->setModel( this->proxy );
    this->ui->rssTableView->setWordWrap( true );

    // set up tabs
    this->ui->tabWidget->setTabIcon( 0, QIcon( ":/icons/results" ));
    this->ui->tabWidget->setTabIcon( 1, QIcon( ":/icons/configure" ));
    this->ui->tabWidget->setTabIcon( 2, QIcon( ":/icons/filter" ));

    // set up filter list
    this->ui->filterView->setModel( this->filterModel );
    this->ui->filterView->setAlternatingRowColors( true );

    // set up tray
    this->trayIcon->show();

    // set up timer
    this->connect( this->timer, SIGNAL( timeout()), this, SLOT( downloadRSS()));

    // read settings
    this->ui->valueMinPrice->setValue( Settings::instance()->value( "filter/minPrice", Ui::DefaultMinPrice ).toInt());
    this->ui->valueMaxPrice->setValue( Settings::instance()->value( "filter/maxPrice", Ui::DefaultMaxPrice ).toInt());
    this->ui->valueMinArea->setValue( Settings::instance()->value( "filter/minArea", Ui::DefaultMinArea ).toInt());
    this->ui->valueMaxArea->setValue( Settings::instance()->value( "filter/maxArea", Ui::DefaultMaxArea ).toInt());
    this->ui->valueFloor->setValue( Settings::instance()->value( "filter/minFloor", Ui::DefaultMinFloor ).toInt());
    this->ui->valueMinRooms->setValue( Settings::instance()->value( "filter/minRooms", Ui::DefaultMinRooms ).toInt());
    this->ui->valueMaxRooms->setValue( Settings::instance()->value( "filter/maxRooms", Ui::DefaultMaxRooms ).toInt());
    this->ui->urlRSS->setText( Settings::instance()->value( "filter/url", Ui::DefaultURL ).toString());

    // report
    this->statusBar()->showMessage( this->tr( "Search has not been started" ));

    // unlock vars
    this->m_lock = false;

    // filter list index change
    this->connect( this->ui->filterView->selectionModel(), &QItemSelectionModel::selectionChanged, [ this ]( const QItemSelection &selected, const QItemSelection & ) {
        if ( !selected.indexes().isEmpty()) {
            if ( selected.indexes().first().isValid()) {
                this->ui->removeButton->setEnabled( true );
                return;
            }
        }
        this->ui->removeButton->setDisabled( true );
    } );

    // connect to RSS resource
    this->connect( this->manager, SIGNAL( finished( QNetworkReply * )), this, SLOT( replyReceived( QNetworkReply * )));
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow() {
    this->disconnect( this->manager, SIGNAL( finished( QNetworkReply * )));
    delete this->manager;
    delete this->timer;
    delete this->trayIcon;
    delete this->filterModel;
    delete this->ui;
}

/**
 * @brief MainWindow::parseRSS
 */
void MainWindow::parseRSS() {
    QString tag;
    Listing listing;
    int count = 0;

    // announce
    while ( !this->xml.atEnd()) {
        this->xml.readNext();

        //bool found = false;
        if ( this->xml.isStartElement()) {
            tag = this->xml.name().toString();
            if ( tag == "item" ) {
                // clear listing
                listing = Listing();
            }
        } else if ( this->xml.isEndElement()) {
            if ( this->xml.name() == "item" && !listing.description().isEmpty()) {
                bool ok = false;

                // must match at least one filter
                foreach ( const Filter &filter, Main::instance()->filters ) {
                    if ( listing.floor() >= filter.floorMin() &&
                         listing.area() >= filter.areaMin() &&
                         listing.area() <= filter.areaMax() &&
                         listing.price() >= filter.priceMin() &&
                         listing.price() <= filter.priceMax() &&
                         listing.rooms() >= filter.roomsMin() &&
                         listing.rooms() <= filter.roomsMax()) {
                        ok = true;
                        break;
                    }
                }

                // apply filters
                if ( ok ) {
                    // check for duplicates
                    bool dup = false;
                    foreach ( const Listing &l, Main::instance()->listings ) {
                        if ( !QString::compare( listing.link(), l.link()))
                            dup = true;
                    }

                    if ( !dup ) {
                        Main::instance()->listings << listing;
                        count++;
                    }
                }
            }
        } else if ( this->xml.isCharacters() && !this->xml.isWhitespace()) {
            if ( !QString::compare( tag, "title" ))
                listing.setDescription( this->xml.text().toString());
            else if ( !QString::compare( tag, "link" ))
                listing.setLink( this->xml.text().toString());
            else if ( !QString::compare( tag, "pubDate" ))
                listing.setDateTime(QLocale( QLocale::English, QLocale::UnitedStates ).toDateTime( this->xml.text().toString().mid( 5, 20 ), "dd MMM yyyy hh:mm:ss" ));
            else if ( !QString::compare( tag, "description" ))
                listing.parseRawXML( this->xml.text().toString());
        }
    }

    // only report if new listings found
    if ( count > 0 ) {
        this->trayIcon->showMessage( this->tr( "FindAFlat" ), this->tr( QString::number( count ).endsWith( "1" ) ? "%1 new listing found" : "%1 new listings found" ).arg( count ), QSystemTrayIcon::Information );

        // goto Listings tab
        this->ui->tabWidget->setCurrentIndex( Listings );

        // raise window
        this->setWindowState( Qt::WindowActive );

        // update table
        this->fillListings();
    }
}

/**
 * @brief MainWindow::replyReceived
 * @param networkReply
 */
void MainWindow::replyReceived( QNetworkReply *networkReply ) {
    if ( !networkReply->error())  {
        const QByteArray data( networkReply->readAll());
        this->xml.clear();
        this->xml.addData( data );

        // parse rss
        this->parseRSS();
    }

    // update statusbar
    this->statusBar()->showMessage( this->tr( "Search in progress (%1)" ).arg( QTime::currentTime().toString( "HH:mm:ss" )));

    // clean up
    networkReply->deleteLater();
}

/**
 * @brief MainWindow::openURL
 * @param index
 */
void MainWindow::openURL( const QModelIndex &index ) {
    // get source model index
    const QModelIndex sourceIndex( this->proxy->mapToSource( index ));
    if ( !index.isValid())
        return;

    // get listing id
    const int id = sourceIndex.row();

    // failsafe
    if ( id < 0 || id >= Main::instance()->listings.count())
        return;

    QDesktopServices::openUrl( QUrl( Main::instance()->listings.at( id ).link()));
}

/**
 * @brief MainWindow::startSearch
 */
void MainWindow::startSearch() {
    this->check();

    // begin polling every minute
    this->downloadRSS();
    this->timer->start( 60000 );
}

/**
 * @brief MainWindow::stopSearch
 */
void MainWindow::stopSearch() {
    this->timer->stop();
    this->statusBar()->showMessage( this->tr( "Search discontinued" ));
}

/**
 * @brief MainWindow::clear
 */
void MainWindow::clear() {
    this->listingModel->beginReset();
    Main::instance()->listings.clear();
    this->listingModel->endReset();
}

/**
 * @brief MainWindow::check
 */
void MainWindow::check() {
    // apply failsafes in filters
    int min = this->ui->valueMinArea->value();
    int max = this->ui->valueMaxArea->value();

    if ( max < min )
        this->ui->valueMaxArea->setValue( min );

    min = this->ui->valueMinPrice->value();
    max = this->ui->valueMaxPrice->value();

    if ( max < min )
        this->ui->valueMaxPrice->setValue( min );

    min = this->ui->valueMinRooms->value();
    max = this->ui->valueMaxRooms->value();

    if ( max < min )
        this->ui->valueMaxRooms->setValue( min );
}

/**
 * @brief MainWindow::on_actionOpen_triggered
 */
void MainWindow::on_actionOpen_triggered() {
    const QString filename( QFileDialog::getOpenFileName( this, this->tr( "Select XML" ), QDir::currentPath(), this->tr( "XML listings (*.xml)" )));

    if ( !filename.isEmpty())
        Main::instance()->readListings( filename );
}

/**
 * @brief MainWindow::on_actionStore_triggered
 */
void MainWindow::on_actionStore_triggered() {
    const QString filename( QFileDialog::getSaveFileName( this, this->tr( "Select XML" ), QDir::currentPath(), this->tr( "XML listings (*.xml)" )));

    if ( !filename.isEmpty())
        Main::instance()->storeListings( filename );
}

/**
 * @brief MainWindow::on_actionSearch_toggled
 * @param state
 */
void MainWindow::on_actionSearch_toggled( bool checked ) {
    if ( this->m_lock )
        return;

    if ( checked ) {
        this->startSearch();
        this->ui->tabWidget->setCurrentIndex( Listings );
        return;
    }

    this->stopSearch();
}

/**
 * @brief MainWindow::on_actionClear_triggered
 */
void MainWindow::on_actionClear_triggered() {
    this->clear();
    this->check();
    this->timer->stop();
}

/**
 * @brief MainWindow::on_resetButton_clicked
 */
void MainWindow::on_resetButton_clicked() {
    this->ui->valueMinPrice->setValue( Ui::DefaultMinPrice );
    this->ui->valueMaxPrice->setValue( Ui::DefaultMaxPrice );
    this->ui->valueMinArea->setValue( Ui::DefaultMinArea );
    this->ui->valueMaxArea->setValue( Ui::DefaultMaxArea );
    this->ui->valueFloor->setValue( Ui::DefaultMinFloor );
    this->ui->valueMinRooms->setValue( Ui::DefaultMinRooms );
    this->ui->valueMaxRooms->setValue( Ui::DefaultMaxRooms );
    this->ui->urlRSS->setText( Ui::DefaultURL );
}

/**
 * @brief MainWindow::fillListings
 */
void MainWindow::fillListings() {
    // reset model
    this->listingModel->beginReset();
    this->listingModel->endReset();

    // scale window to contents
    this->ui->rssTableView->resizeColumnsToContents();
    this->ui->rssTableView->resizeRowsToContents();

    // sort
    this->listingModel->sort( ListingModel::Time, Qt::AscendingOrder );
}

/**
 * @brief MainWindow::setupFilters
 */
void MainWindow::setupFilters() {
    int y;

    this->filterModel->beginReset();

    // read filters
    for ( y = 0; y < Settings::instance()->value( "filter/numFilters", 0 ).toInt(); y++ )
        Main::instance()->filters << Filter::fromString( Settings::instance()->value( QString( "filter/filter_%1" ).arg( y )).toString());

    this->filterModel->endReset();

    // restore listings if any
    Main::instance()->readListings();
}

/**
 * @brief MainWindow::checkButtonStates
 */
void MainWindow::checkButtonStates() {
    const bool hasFilters = !Main::instance()->filters.isEmpty();

    this->ui->actionSearch->setEnabled( hasFilters );
    this->ui->removeAllButton->setEnabled( hasFilters );
    this->ui->removeButton->setEnabled( hasFilters && this->ui->filterView->currentIndex().isValid());
}

/**
 * @brief MainWindow::on_addButton_clicked
 */
void MainWindow::on_addButton_clicked() {
    this->filterModel->beginReset();
    Main::instance()->filters << Filter( this->priceMin(), this->priceMax(), this->areaMin(), this->areaMax(), this->floorMin(), this->roomsMin(), this->roomsMax());
    this->filterModel->endReset();

    // goto to Filters tab
    this->ui->tabWidget->setCurrentIndex( Filters );
}

/**
 * @brief MainWindow::on_removeButton_clicked
 */
void MainWindow::on_removeButton_clicked() {
    this->filterModel->beginReset();

    const int row = this->ui->filterView->currentIndex().row();
    if ( row >=0 && row < Main::instance()->filters.count())
        Main::instance()->filters.takeAt( this->ui->filterView->currentIndex().row());

    this->filterModel->endReset();
}

/**
 * @brief MainWindow::on_removeAllButton_clicked
 */
void MainWindow::on_removeAllButton_clicked(){
    this->filterModel->beginReset();
    Main::instance()->filters.clear();
    this->filterModel->endReset();

    // return to FilerOptions tab
    this->ui->tabWidget->setCurrentIndex( FilterOptions );
}
