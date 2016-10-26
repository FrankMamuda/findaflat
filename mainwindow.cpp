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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopServices>
#include <QTimer>
#include <QSettings>
#include <QFileDialog>
#include <QXmlStreamReader>
#include "main.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow ), m_lock( true ) {
    ui->setupUi( this );

    // define column headers here for now (required for proper translations)
    this->columnHeaders <<
                           this->tr( "Address" ) <<
                           this->tr( "Rooms" ) <<
                           this->tr( "Area" ) <<
                           this->tr( "Floor" ) <<
                           this->tr( "Date" ) <<
                           this->tr( "Price" );

    // set up ranking view
    this->ui->rssTableView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    this->ui->rssTableView->setSortingEnabled( true );

    // set up sorting & view model
    this->proxyModel = new ListingSortModel( this );
    this->modelPtr = new QStandardItemModel( m.listingList.count(), this->columnHeaders.count(), this );
    this->proxyModel->setSourceModel( this->modelPtr );
    this->proxyModel->setDynamicSortFilter( true );
    this->ui->rssTableView->verticalHeader()->hide();
    this->ui->rssTableView->setModel( this->proxyModel );
    this->ui->rssTableView->setWordWrap( true );

    // set up tabs
    this->ui->tabWidget->setTabIcon( 0, QIcon( ":/icons/results" ));
    this->ui->tabWidget->setTabIcon( 1, QIcon( ":/icons/configure" ));
    this->ui->tabWidget->setTabIcon( 2, QIcon( ":/icons/filter" ));

    // fill header
    this->fillHeader();

    // set up filter list
    this->filterModel = new FilterModel( this );
    this->ui->filterView->setModel( this->filterModel );
    this->ui->filterView->setAlternatingRowColors( true );

    // set up tray
    QIcon icon( ":/icons/icon" );
    this->trayIcon = new QSystemTrayIcon( icon, this );
    this->trayIcon->show();

    // set up timer
    this->timer = new QTimer( this );
    this->connect( this->timer, SIGNAL( timeout()), this, SLOT( downloadRSS()));

    // read settings
    this->ui->valueMinPrice->setValue( m.settings->value( "filter/minPrice", Ui::DefaultMinPrice ).toInt());
    this->ui->valueMaxPrice->setValue( m.settings->value( "filter/maxPrice", Ui::DefaultMaxPrice ).toInt());
    this->ui->valueMinArea->setValue( m.settings->value( "filter/minArea", Ui::DefaultMinArea ).toInt());
    this->ui->valueMaxArea->setValue( m.settings->value( "filter/maxArea", Ui::DefaultMaxArea ).toInt());
    this->ui->valueFloor->setValue( m.settings->value( "filter/minFloor", Ui::DefaultMinFloor ).toInt());
    this->ui->valueMinRooms->setValue( m.settings->value( "filter/minRooms", Ui::DefaultMinRooms ).toInt());
    this->ui->valueMaxRooms->setValue( m.settings->value( "filter/maxRooms", Ui::DefaultMaxRooms ).toInt());
    this->ui->urlRSS->setText( m.settings->value( "filter/url", Ui::DefaultURL ).toString());

    // read filters
    int y, count = m.settings->value( "filter/numFilters", 0 ).toInt();
    for ( y = 0; y < count; y++ ) {
        Filter *filter = Filter::fromString( m.settings->value( QString( "filter/filter_%1" ).arg( y )).toString());
        if ( filter != NULL )
            m.filterList << filter;
    }

    // read previous listings
    this->readListings();

    // report
    this->statusBar()->showMessage( this->tr( "Search has not been started" ));

    // unlock vars
    this->m_lock = false;
}

/**
 * @brief MainWindow::fillHeader
 */
void MainWindow::fillHeader() {
    QFont boldFont;
    int y;

    boldFont.setBold( true );

    // generate header
    for ( y = 0; y < this->columnHeaders.count(); y++ ) {
        QStandardItem *itemPtr = new QStandardItem( this->columnHeaders.at( y ));
        itemPtr->setFont( boldFont );
        itemPtr->setData( Qt::AlignCenter, Qt::TextAlignmentRole );
        this->modelPtr->setHorizontalHeaderItem( y, itemPtr );
    }
}

/**
 * @brief MainWindow::fillData
 */
void MainWindow::fillData() {
    int y, k;

    // fill data
    for ( y = 0; y < this->columnHeaders.count(); y++ ) {
        for ( k = 0; k < m.listingList.count(); k++ ) {
            Listing *listing = m.listingList.at( k );
            QStandardItem *item = new QStandardItem();
            QString text;

            if ( listing == NULL )
                return;

            switch ( y ) {
            case Address:
                text = listing->address();
                break;

            case Rooms:
                text = QString( "%1" ).arg( listing->rooms());
                break;

            case Area:
                text = QString( "%1" ).arg( listing->area());
                break;

            case Floor:
                text = QString( "%1" ).arg( listing->floor());
                break;

            case Price:
                text = QString( "%1" ).arg( listing->price());
                break;

            case DateTime:
                text = listing->dateTime().toString( "MMM dd hh:mm" );
                break;

            default:
                break;
            }

            // make it centred
            item->setData( Qt::AlignCenter, Qt::TextAlignmentRole );
            item->setText( text );
            item->setData( m.listingList.indexOf( listing ), Qt::UserRole );
            this->modelPtr->setItem( k, y, item );
        }
    }

    // scale window to contents
    this->ui->rssTableView->resizeColumnsToContents();
    this->ui->rssTableView->resizeRowsToContents();
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow() {
    // store previous listings
    this->storeListings();

    // store settings
    m.settings->setValue( "filter/minPrice", this->priceMin());
    m.settings->setValue( "filter/maxPrice", this->priceMax());
    m.settings->setValue( "filter/minArea", this->areaMin());
    m.settings->setValue( "filter/maxArea", this->areaMax());
    m.settings->setValue( "filter/minFloor", this->floorMin());
    m.settings->setValue( "filter/minRooms", this->roomsMin());
    m.settings->setValue( "filter/maxRooms", this->roomsMax());
    m.settings->setValue( "filter/url", this->ui->urlRSS->text());

    // store filters
    int count = 0;
    foreach ( Filter *filter, m.filterList ) {
        m.settings->setValue( QString( "filter/filter_%1" ).arg( count ), filter->settingsString());
        count++;
        delete filter;
    }
    m.settings->setValue( "filter/numFilters", count );

    // delete ui & other stuff
    delete this->timer;
    delete this->trayIcon;
    delete this->filterModel;
    delete ui;
    delete m.settings;

    // clear listings
    foreach ( Listing *listing, m.listingList )
        delete listing;
}

/**
 * @brief MainWindow::parseRSS
 */
void MainWindow::parseRSS() {
    QString tag;
    QString link;
    QString title;
    QString date;
    QString description;
    Listing *listing;
    int count = 0;

    // announce
    //qDebug() << QString( "%1: parsing RSS from %2" ).arg( QTime::currentTime().toString( "HH:mm:ss" )).arg( this->ui->urlRSS->text());

    while ( !this->xml.atEnd()) {
        this->xml.readNext();

        if ( this->xml.isStartElement()) {
            if ( this->xml.name() == "item" ) {
                if ( !title.isEmpty())
                    listing = new Listing();
            }
            tag = this->xml.name().toString();
        } else if( this->xml.isEndElement()) {
            if ( this->xml.name() == "item" ) {
                bool ok = false;

                // read listing info
                listing->parseRawXML( description );
                listing->setLink( link );
                listing->setDescription( title );

                // parse pubDate tag
                QDateTime dateTime;
                date = date.mid( 5, 20 );
                QLocale locale( QLocale::English, QLocale::UnitedStates );
                dateTime = locale.toDateTime( date, "dd MMM yyyy hh:mm:ss" );
                listing->setDateTime( dateTime );

                // must match at least one filter
                foreach ( Filter *filter, m.filterList ) {
                    if ( listing->floor() >= filter->floorMin() &&
                         listing->area() >= filter->areaMin() &&
                         listing->area() <= filter->areaMax() &&
                         listing->price() >= filter->priceMin() &&
                         listing->price() <= filter->priceMax() &&
                         listing->rooms() >= filter->roomsMin() &&
                         listing->rooms() <= filter->roomsMax()) {
                        ok = true;
                        break;
                    }
                }

                // apply filters
                if ( !ok ) {
                    delete listing;
                } else {
                    // check for duplicates
                    bool found = false;
                    foreach ( Listing *listingPtr, m.listingList ) {
                        if ( !QString::compare( listingPtr->link(), listing->link()))
                            found = true;
                    }

                    if ( !found ) {
                        m.listingList << listing;
                        count++;
                    }
                }
            }
        } else if ( this->xml.isCharacters() && !this->xml.isWhitespace()) {
            if ( tag == "title" )
                title = this->xml.text().toString();
            else if ( tag == "link" )
                link = this->xml.text().toString();
            else if ( tag == "pubDate" )
                date = this->xml.text().toString();
            else if ( tag == "description" )
                description = this->xml.text().toString();
        }
    }

    // only report if new listings found
    if ( count > 0 ) {
        QString num = QString( "%1" ).arg( count );

        if ( num.endsWith( "1" ))
            this->trayIcon->showMessage( this->tr( "FindAFlat" ), this->tr( "%1 new listing found" ).arg( count ), QSystemTrayIcon::Information );
        else
            this->trayIcon->showMessage( this->tr( "FindAFlat" ), this->tr( "%1 new listings found" ).arg( count ), QSystemTrayIcon::Information );

        // raise window
        this->setWindowState( Qt::WindowActive );

        // update table
        this->clearData();
        this->fillData();
        this->proxyModel->sort( DateTime, Qt::AscendingOrder );
    }
}

/**
 * @brief MainWindow::downloadRSS
 */
void MainWindow::downloadRSS() {
    // connect to RSS resource
    this->manager = new QNetworkAccessManager( this );
    this->connect( this->manager, SIGNAL( finished( QNetworkReply * )), this, SLOT( replyReceived( QNetworkReply * )));
    this->manager->get( QNetworkRequest( QUrl( this->ui->urlRSS->text())));
}

/**
 * @brief MainWindow::replyReceived
 * @param networkReply
 */
void MainWindow::replyReceived( QNetworkReply *networkReply ) {
    if ( !networkReply->error())  {
        QByteArray data = networkReply->readAll();
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
    int id;
    Listing *listing;

    // get listing id
    id = this->proxyModel->data( index, Qt::UserRole ).toInt();

    // failsafe
    if ( id < 0 || id >= m.listingList.count())
        return;

    listing = m.listingList.at( id );
    if ( &listing != NULL )
        QDesktopServices::openUrl( QUrl( listing->link()));
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
    m.listingList.clear();
    this->clearData();
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
    QString filename = QFileDialog::getOpenFileName( this, this->tr( "Select XML" ), QDir::currentPath(), this->tr( "XML listings (*.xml)" ));
    if ( !filename.isEmpty())
        this->readListings( filename );
}

/**
 * @brief MainWindow::on_actionStore_triggered
 */
void MainWindow::on_actionStore_triggered() {
    QString filename = QFileDialog::getSaveFileName( this, this->tr( "Select XML" ), QDir::currentPath(), this->tr( "XML listings (*.xml)" ));
    if ( !filename.isEmpty())
        this->storeListings( filename );
}

/**
 * @brief MainWindow::on_actionSearch_toggled
 * @param state
 */
void MainWindow::on_actionSearch_toggled( bool checked ) {
    if ( this->m_lock )
        return;

    if ( checked )
        this->startSearch();
    else
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
 * @brief MainWindow::storeListings
 */
void MainWindow::storeListings( const QString &path ) {
    QFile xmlFile( path );
    if ( xmlFile.open( QFile::ReadWrite | QFile::Truncate )) {
        xmlFile.write( "<listings version=\"1.0\">\n" );

        foreach ( Listing *listing, m.listingList ) {
            xmlFile.write( QString( "<listing rooms=\"%1\" price=\"%2\" area=\"%3\" floor=\"%4\" total=\"%5\" description=\"%6\" link=\"%7\" address=\"%8\" date=\"%9\" />\n" )
                           .arg( listing->rooms())
                           .arg( listing->price())
                           .arg( listing->area())
                           .arg( listing->floor())
                           .arg( listing->totalFloors())
                           .arg( listing->description())
                           .arg( listing->link())
                           .arg( listing->address())
                           .arg( listing->dateTime().toString( "dd.MM.yyyy hh:mm" )).toUtf8()
                           );
        }

        xmlFile.write( "</listings>\n" );
        xmlFile.close();
    }
}

/**
 * @brief MainWindow::readListings
 */
void MainWindow::readListings( const QString &path ) {
    QFile xmlFile( path );
    if ( xmlFile.open( QFile::ReadOnly )) {
        QXmlStreamReader xml;
        xml.addData( xmlFile.readAll());

        // clear table before adding new entries
        this->clear();

        if ( xml.readNextStartElement()) {
            if ( xml.name() == "listings" && xml.attributes().value( "version" ) == "1.0" ) {
                while ( xml.readNextStartElement()) {
                    if ( xml.name() == "listing" ) {
                        Listing *listing = new Listing();
                        listing->setRooms( xml.attributes().value( "rooms" ).toInt());
                        listing->setPrice( xml.attributes().value( "price" ).toInt());
                        listing->setArea( xml.attributes().value( "area" ).toInt());
                        listing->setFloor( xml.attributes().value( "floor" ).toInt());
                        listing->setTotalFloors( xml.attributes().value( "total" ).toInt());
                        listing->setDescription( xml.attributes().value( "description" ).toString());
                        listing->setLink( xml.attributes().value( "link" ).toString());
                        listing->setAddress( xml.attributes().value( "address" ).toString());
                        listing->setDateTime( QDateTime::fromString( xml.attributes().value( "date" ).toString(), "dd.MM.yyyy hh:mm" ));

                        // check for duplicates
                        bool found = false;
                        foreach ( Listing *listingPtr, m.listingList ) {
                            if ( !QString::compare( listingPtr->link(), listing->link()))
                                found = true;
                        }

                        if ( !found )
                            m.listingList << listing;

                        xml.readNext();
                    } else
                        xml.skipCurrentElement();
                }
            } else
                xml.raiseError( this->tr( "invalid file or version" ));
        }

        // table
        this->clearData();
        this->fillData();
        this->proxyModel->sort( DateTime, Qt::AscendingOrder );

        // close file
        xmlFile.close();
    }
}

/**
 * @brief ListingSortModel::lessThan
 * @param left
 * @param right
 * @return
 */
bool ListingSortModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const {
    QVariant leftData = this->sourceModel()->data( left );
    QVariant rightData = this->sourceModel()->data( right );
    bool n1, n2;
    int num1, num2;

    // special compare for dates
    if ( left.column() == MainWindow::DateTime ) {
        int leftId = this->sourceModel()->data( left, Qt::UserRole ).toInt();
        int rightId = this->sourceModel()->data( right, Qt::UserRole ).toInt();

        if ( leftId >= 0 && leftId < m.listingList.count() && rightId >= 0 && rightId < m.listingList.count()) {
            Listing *leftL = m.listingList.at( leftId );
            Listing *rightL = m.listingList.at( rightId );

            if ( leftL != NULL && rightL != NULL )
                return leftL->dateTime() < rightL->dateTime();
        }
    }

    // other columns contain either text or number
    num1 = leftData.toInt( &n1 );
    num2 = rightData.toInt( &n2 );

    if ( n1 && n2 )
        return num1 < num2;

    return QString::localeAwareCompare( leftData.toString(), rightData.toString()) < 0;
}

/**
 * @brief MainWindow::on_addButton_clicked
 */
void MainWindow::on_addButton_clicked() {
    this->filterModel->beginReset();
    m.filterList << new Filter( this->priceMin(), this->priceMax(), this->areaMin(), this->areaMax(), this->floorMin(), this->roomsMin(), this->roomsMax());
    this->filterModel->endReset();
}

/**
 * @brief MainWindow::on_removeButton_clicked
 */
void MainWindow::on_removeButton_clicked() {
    int row;

    this->filterModel->beginReset();

    row = this->ui->filterView->currentIndex().row();
    if ( row >=0 && row < m.filterList.count()) {
        Filter *filter = m.filterList.takeAt( this->ui->filterView->currentIndex().row());
        delete filter;
    }

    this->filterModel->endReset();
}

/**
 * @brief MainWindow::on_removeAllButton_clicked
 */
void MainWindow::on_removeAllButton_clicked(){
    this->filterModel->beginReset();

    foreach ( Filter *filter, m.filterList )
        delete filter;

    m.filterList.clear();

    this->filterModel->endReset();
}
