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
#include "listing.h"

/**
 * @brief The Filter class
 */
class Filter {
public:
    explicit Filter( int priceMin, int priceMax, int areaMin, int areaMax,
                     int floorMin, int roomsMin, int roomsMax )
        : m_priceMin( priceMin ), m_priceMax( priceMax ), m_areaMin( areaMin ), m_areaMax( areaMax ),
          m_floorMin( floorMin ), m_roomsMin( roomsMin ), m_roomsMax( roomsMax ) {}
    int priceMin() const { return this->m_priceMin;  }
    int priceMax() const { return this->m_priceMax;  }
    int areaMin() const  { return this->m_areaMin;   }
    int areaMax() const  { return this->m_areaMax;   }
    int floorMin() const { return this->m_floorMin; }
    int roomsMin() const { return this->m_roomsMin;  }
    int roomsMax() const { return this->m_roomsMax;  }
    bool compare( const Listing *listing ) const;
    QString settingsString() const;
    static Filter *fromString( const QString &filter );

private:
    int m_priceMin;
    int m_priceMax;
    int m_areaMin;
    int m_areaMax;
    int m_floorMin;
    int m_roomsMin;
    int m_roomsMax;
};
