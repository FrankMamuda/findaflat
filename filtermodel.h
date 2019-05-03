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
#include <QStringListModel>
#include "main.h"

/**
 * @brief The FilterModel class
 */
class FilterModel : public QStringListModel {
    Q_OBJECT

public:
    explicit FilterModel( QObject *parentPtr = 0 ) : QStringListModel( parentPtr ) {}
    int rowCount( const QModelIndex & = QModelIndex()) const override { return Main::instance()->filterList.count(); }
    QVariant data( const QModelIndex &, int ) const override;
    void beginReset() { this->beginResetModel(); }
    void endReset() { this->endResetModel(); }
};
