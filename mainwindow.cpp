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
                           this->tr( "Adrese" ) <<
                           this->tr( "Istabas" ) <<
                           this->tr( "Platība" ) <<
                           this->tr( "Stāvs" ) <<
                           this->tr( "Datums" ) <<
                           this->tr( "Cena" );

    // set up ranking view
    this->ui->rssTableView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    this->ui->rssTableView->setSortingEnabled( true );

    // set up sorting & view model
    this->proxyModel = new FlatSortModel( this );
    this->modelPtr = new QStandardItemModel( m.flatList.count(), this->columnHeaders.count(), this );
    this->proxyModel->setSourceModel( this->modelPtr );
    this->proxyModel->setDynamicSortFilter( true );
    this->ui->rssTableView->verticalHeader()->hide();
    this->ui->rssTableView->setModel( this->proxyModel );
    this->ui->rssTableView->setWordWrap( true );

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
    this->statusBar()->showMessage( this->tr( "Meklēšana nav uzsākta" ));

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
        for ( k = 0; k < m.flatList.count(); k++ ) {
            Flat *flat = m.flatList.at( k );
            QStandardItem *item = new QStandardItem();
            QString text;

            if ( flat == NULL )
                return;

            switch ( y ) {
            case Address:
                text = flat->address();
                break;

            case Rooms:
                text = QString( "%1" ).arg( flat->rooms());
                break;

            case Area:
                text = QString( "%1" ).arg( flat->area());
                break;

            case Floor:
                text = QString( "%1" ).arg( flat->floor());
                break;

            case Price:
                text = QString( "%1" ).arg( flat->price());
                break;

            case DateTime:
                text = flat->dateTime().toString( "MMM dd hh:mm" );
                break;

            default:
                break;
            }

            // make it centred
            item->setData( Qt::AlignCenter, Qt::TextAlignmentRole );
            item->setText( text );
            item->setData( m.flatList.indexOf( flat ), Qt::UserRole );
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
    foreach ( Flat *flat, m.flatList )
        delete flat;
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
    Flat *flat;
    int count = 0;

    // announce
    qDebug() << QString( "%1: parsing RSS from %2" ).arg( QTime::currentTime().toString( "HH:mm:ss" )).arg( this->ui->urlRSS->text());

    while ( !this->xml.atEnd()) {
        this->xml.readNext();

        if ( this->xml.isStartElement()) {
            if ( this->xml.name() == "item" ) {
                if ( !title.isEmpty())
                    flat = new Flat();
            }
            tag = this->xml.name().toString();
        } else if( this->xml.isEndElement()) {
            if ( this->xml.name() == "item" ) {
                // read listing info
                flat->parseRawXML( description );
                flat->setLink( link );
                flat->setDescription( title );

                // (\\d{1,2}\\s+(?:Jan?|Feb?|Mar?|Apr?|May|Jun?|Jul?|Aug?|Sep?|Oct?|Nov?|Dec?)\\s+\\d{4})\\s(\\d{2}:\\d{2}:\\d{2})

                QDateTime dateTime;
                date = date.mid( 5, 20 );
                QLocale locale( QLocale::English, QLocale::UnitedStates );
                dateTime = locale.toDateTime( date, "dd MMM yyyy hh:mm:ss" );
                flat->setDateTime( dateTime );
                //qDebug() << dateTime.toString( "MMM dd hh:mm" );

                bool ok = false;
                foreach ( Filter *filter, m.filterList ) {
                    if ( flat->floor() >= filter->floorMin() &&
                         flat->area() >= filter->areaMin() &&
                         flat->area() <= filter->areaMax() &&
                         flat->price() >= filter->priceMin() &&
                         flat->price() <= filter->priceMax() &&
                         flat->rooms() >= filter->roomsMin() &&
                         flat->rooms() <= filter->roomsMax()) {
                        ok = true;
                        break;
                    }
                }

                // apply filters
                if ( !ok ) {
                    delete flat;
                } else {
                    // check for duplicates
                    bool found = false;
                    foreach ( Flat *flatPtr, m.flatList ) {
                        if ( !QString::compare( flatPtr->link(), flat->link()))
                            found = true;
                    }

                    if ( !found ) {
                        m.flatList << flat;
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
        qDebug() << QString( "%1: %2 new listings found" ).arg( QTime::currentTime().toString( "HH:mm:ss" )).arg( count );
        //this->trayIcon->showMessage( "FindAFlat", QString( "%1 new listing(s) found" ).arg( count ), QSystemTrayIcon::Information );
        QString num = QString( "%1" ).arg( count );

        if ( num.endsWith( "1" ))
            this->trayIcon->showMessage( "FindAFlat", QString( "%1 jauns sludinājums atrasts" ).arg( count ), QSystemTrayIcon::Information );
        else
            this->trayIcon->showMessage( "FindAFlat", QString( "%1 jauni sludinājumi atrasti" ).arg( count ), QSystemTrayIcon::Information );

        this->show();

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
    this->statusBar()->showMessage( this->tr( "Notiek meklēšana (%1)" ).arg( QTime::currentTime().toString( "HH:mm:ss" )));

    networkReply->deleteLater();
}

/**
 * @brief MainWindow::openURL
 * @param index
 */
void MainWindow::openURL( const QModelIndex &index ) {
    int id = this->proxyModel->data( index, Qt::UserRole ).toInt();

    if ( id < 0 || id >= m.flatList.count())
        return;

    Flat *flat = m.flatList.at( id );
    if ( flat != NULL )
        QDesktopServices::openUrl( QUrl( flat->link()));
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
    this->statusBar()->showMessage( this->tr( "Meklēšana pārtraukta" ));
}

/**
 * @brief MainWindow::clear
 */
void MainWindow::clear() {
    m.flatList.clear();
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
    QString filename = QFileDialog::getOpenFileName( this, this->tr( "Select XML" ), QDir::currentPath(), this->tr( "FindAFlat XML (*.xml)" ));
    if ( !filename.isEmpty())
        this->readListings( filename );
}

/**
 * @brief MainWindow::on_actionStore_triggered
 */
void MainWindow::on_actionStore_triggered() {
    QString filename = QFileDialog::getSaveFileName( this, this->tr( "Select XML" ), QDir::currentPath(), this->tr( "FindAFlat XML (*.xml)" ));
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
        xmlFile.write( "<flats version=\"1.0\">\n" );

        foreach ( Flat *flat, m.flatList ) {
            xmlFile.write( QString( "<flat rooms=\"%1\" price=\"%2\" area=\"%3\" floor=\"%4\" total=\"%5\" description=\"%6\" link=\"%7\" address=\"%8\" date=\"%9\" />\n" )
                           .arg( flat->rooms())
                           .arg( flat->price())
                           .arg( flat->area())
                           .arg( flat->floor())
                           .arg( flat->totalFloors())
                           .arg( flat->description())
                           .arg( flat->link())
                           .arg( flat->address())
                           .arg( flat->dateTime().toString( "dd.MM.yyyy hh:mm" )).toUtf8()
                           );
        }

        xmlFile.write( "</flats>\n" );
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

        this->clear();

        if ( xml.readNextStartElement()) {
            if ( xml.name() == "flats" && xml.attributes().value( "version" ) == "1.0" ) {
                while ( xml.readNextStartElement()) {
                    if ( xml.name() == "flat" ) {
                        Flat *flat = new Flat();
                        flat->setRooms( xml.attributes().value( "rooms" ).toInt());
                        flat->setPrice( xml.attributes().value( "price" ).toInt());
                        flat->setArea( xml.attributes().value( "area" ).toInt());
                        flat->setFloor( xml.attributes().value( "floor" ).toInt());
                        flat->setTotalFloors( xml.attributes().value( "total" ).toInt());
                        flat->setDescription( xml.attributes().value( "description" ).toString());
                        flat->setLink( xml.attributes().value( "link" ).toString());
                        flat->setAddress( xml.attributes().value( "address" ).toString());
                        flat->setDateTime( QDateTime::fromString( xml.attributes().value( "date" ).toString(), "dd.MM.yyyy hh:mm" ));

                        // check for duplicates
                        bool found = false;
                        foreach ( Flat *flatPtr, m.flatList ) {
                            if ( !QString::compare( flatPtr->link(), flat->link()))
                                found = true;
                        }

                        if ( !found )
                            m.flatList << flat;

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
 * @brief FlatSortModel::lessThan
 * @param left
 * @param right
 * @return
 */
bool FlatSortModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const {
    QVariant leftData = this->sourceModel()->data( left );
    QVariant rightData = this->sourceModel()->data( right );
    bool n1, n2;
    int num1, num2;

    if ( left.column() == MainWindow::DateTime ) {
        int leftId = this->sourceModel()->data( left, Qt::UserRole ).toInt();
        int rightId = this->sourceModel()->data( right, Qt::UserRole ).toInt();

        if ( leftId >= 0 && leftId < m.flatList.count() && rightId >= 0 && rightId < m.flatList.count()) {
            Flat *leftFlat = m.flatList.at( leftId );
            Flat *rightFlat = m.flatList.at( rightId );

            if ( leftFlat != NULL && rightFlat != NULL )
                return leftFlat->dateTime() < rightFlat->dateTime();
        }
    }

    num1 = leftData.toInt( &n1 );
    num2 = rightData.toInt( &n2 );

    // these must be integers or strings
    if ( n1 && n2 )
        return num1 < num2;
    else
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
    this->filterModel->beginReset();
    int row = this->ui->filterView->currentIndex().row();

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
