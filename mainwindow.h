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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "flat.h"

namespace Ui {
class MainWindow;
static const unsigned int DefaultMinPrice = 10000;
static const unsigned int DefaultMaxPrice = 40000;
static const unsigned int DefaultMinArea = 40;
static const unsigned int DefaultMaxArea = 60;
static const unsigned int DefaultMinFloor = 2;
static const unsigned int DefaultMinRooms = 1;
static const unsigned int DefaultMaxRooms = 3;
static const QString DefaultURL = "https://www.ss.lv/lv/real-estate/flats/riga/centre/rss/";
}

/**
 * @brief The FlatSortModel class
 */
class FlatSortModel : public QSortFilterProxyModel {
public:
    FlatSortModel( QObject *parent ) : QSortFilterProxyModel( parent ) {}
    bool lessThan( const QModelIndex &left, const QModelIndex &right ) const {
        QVariant leftData = this->sourceModel()->data( left );
        QVariant rightData = sourceModel()->data( right );
        bool n1, n2;
        int num1, num2;

        num1 = leftData.toInt( &n1 );
        num2 = rightData.toInt( &n2 );

        // these must be integers or strings
        if ( n1 && n2 )
            return num1 < num2;
        else
            return QString::localeAwareCompare( leftData.toString(), rightData.toString()) < 0;
    }
};

/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();


private slots:
    void replyReceived( QNetworkReply *reply );
    void downloadRSS();
    void parseRSS();
    //void on_rssView_doubleClicked( const QModelIndex &index) { this->openURL( index ); }
    void clear();
    void check();
    void openURL( const QModelIndex &index );
    void on_openLinkButton_clicked();
    void on_actionOpen_triggered();
    void on_actionStore_triggered();
    void on_actionSearch_toggled( bool checked );
    void startSearch();
    void stopSearch();
    void on_actionClear_triggered();
    void on_resetButton_clicked();
    void storeListings( const QString &path = QDir::currentPath() + "/saved.xml" );
    void readListings( const QString &path = QDir::currentPath() + "/saved.xml" );
    void fillHeader();
    void clearData();
    void fillData();

    void on_rssTableView_doubleClicked(const QModelIndex &index);

private:
    enum Columns {
        Address = 0,
        Rooms,
        Area,
        Floor,
        Price
    };

    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QXmlStreamReader xml;
    QSystemTrayIcon *trayIcon;
    //FlatListModel *flatViewModel;
    QTimer *timer;

    // getters
    int priceMin() const;
    int priceMax() const;
    int areaMin() const;
    int areaMax() const;
    int floorMin() const;
    int roomsMin() const;
    int roomsMax() const;
    int m_lock;

    // table
    QStringList columnHeaders;
    QStandardItemModel *modelPtr;
    FlatSortModel *proxyModel;
};

#endif // MAINWINDOW_H
