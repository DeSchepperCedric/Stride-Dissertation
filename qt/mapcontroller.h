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

#include <QtPositioning/QGeoRectangle>

#include "location.h"

namespace visualization {

    class MapController : public QObject {
    Q_OBJECT
        Q_PROPERTY(QString day READ getDay WRITE setDay)
        Q_PROPERTY(QString naam READ getNaam)
        Q_PROPERTY(QString info READ getInfo)
        Q_PROPERTY(QString ID READ getID WRITE setID)

    public:
        MapController() = default;

        explicit MapController(const std::string &file);

        void setDay(const QString &day);

        QString getDay();

        void initialize(QObject *root);

        void Update();

        Q_INVOKABLE void center();

        QString getInfo();

        QString getNaam();

        void setID(const QString &id);

        QString getID();

    private:
        int m_day = 0;
        int m_id = -1;

        std::vector<Location> m_locations;

        QObject *m_root = nullptr;
    };

}


#endif // MAPCONTROLLER_H
