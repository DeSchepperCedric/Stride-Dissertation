/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2019, Jan Broeckhove and Bistromatics group.
 */

#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>
#include <QString>
#include <iostream>

#include "location.h"

namespace visualization {

class MapController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString day READ getDay WRITE setDay NOTIFY dayChanged)

public:
    MapController() = default;

    MapController(const std::string& file);

    void setDay(const QString& day);
    QString getDay();

    void initialize(QObject* root);

    Q_INVOKABLE void test(){std::cout << "lol" << std::endl;}

signals:
    void dayChanged();

private:
    int m_day = 0;

    std::vector<Location> m_locations;

    QObject* m_root = nullptr;
};

}


#endif // MAPCONTROLLER_H
