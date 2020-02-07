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
#include <QtWidgets/QMainWindow>
#include <QXmlStreamReader>
#include <QFile>
#include <QtNetwork>
#include <QList>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QSettings>
#include <QStandardItemModel>
#include "listing.h"
#include "filtermodel.h"
#include "ui_mainwindow.h"
#include "listingmodel.h"

namespace Ui {
class MainWindow;
static const unsigned int DefaultMinPrice = 10000;
static const unsigned int DefaultMaxPrice = 40000;
static const unsigned int DefaultMinArea = 40;
static const unsigned int DefaultMaxArea = 60;
static const unsigned int DefaultMinFloor = 2;
static const unsigned int DefaultMinRooms = 1;
static const unsigned int DefaultMaxRooms = 3;
static const QString DefaultURL = "https://www.ss.com/lv/real-estate/flats/riga/centre/rss/";
}

/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    friend class Settings;

public:
    enum Tabs {
        Listings,
        FilterOptions,
        Filters
    };

    static MainWindow *instance() { static MainWindow *instance( new MainWindow()); return instance; }
    ~MainWindow();

public slots:
    void fillListings();
    void setupFilters();
    void checkButtonStates();
    void sendNotificationToFirebase( const QString &message );
    void sendDataToFirebase( const QString &name, const QString &url, const QString &imageUrl );

private slots:
    void replyReceived( QNetworkReply *reply );
    void downloadRSS() { if ( this->manager != nullptr ) this->manager->get( QNetworkRequest( QUrl( this->ui->urlRSS->text()))); }
    void parseRSS();
    void clear();
    void check();
    void openURL( const QModelIndex &index );
    void on_openLinkButton_clicked() { this->openURL( this->ui->rssTableView->currentIndex()); }
    void on_actionOpen_triggered();
    void on_actionStore_triggered();
    void on_actionSearch_toggled( bool checked );
    void startSearch();
    void stopSearch();
    void on_actionClear_triggered();
    void on_resetButton_clicked();
    void on_rssTableView_doubleClicked( const QModelIndex &index ) { this->openURL( index ); }
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_removeAllButton_clicked();

private:
    explicit MainWindow( QWidget *parent = nullptr );

    Ui::MainWindow *ui;
    QNetworkAccessManager *manager = new QNetworkAccessManager( this );
    QXmlStreamReader xml;
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon( QIcon( ":/icons/icon" ), this );
    QTimer *timer = new QTimer( this );
    int m_lock = true;

    // getters
    int priceMin() const { return this->ui->valueMinPrice->value(); }
    int priceMax() const { return this->ui->valueMaxPrice->value(); }
    int areaMin() const { return this->ui->valueMinArea->value(); }
    int areaMax() const { return this->ui->valueMaxArea->value(); }
    int floorMin() const { return this->ui->valueFloor->value(); }
    int roomsMin() const { return this->ui->valueMinRooms->value(); }
    int roomsMax() const { return this->ui->valueMaxRooms->value(); }
    QString url() const { return this->ui->urlRSS->text(); }
    bool notificationsEnabled() const { return this->ui->notificationsCheck->isChecked(); }
    QString token() const { return this->ui->tokenEdit->text(); }
    QString topic() const { return this->ui->topicEdit->text(); }

    // table
    ListingModel *listingModel = new ListingModel( this );
    FilterModel *filterModel = new FilterModel( this );
    QSortFilterProxyModel *proxy = new QSortFilterProxyModel();
};
