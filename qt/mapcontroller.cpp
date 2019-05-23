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

#include <QVariant>
#include <fstream>
#include <memory>

#include "mapcontroller.h"
#include "../main/cpp/geopop/io/EpiJSONReader.h"

#include <unistd.h>

namespace visualization {

    using namespace std;

    MapController::MapController(const std::string &file_name) : QObject(nullptr) {
        std::string temp = file_name;
        auto file = std::make_unique<std::ifstream>();
        file->open("/home/wannes/Desktop/UA/3e_bach/BP/Stride-Dissertation/temp.json");
        geopop::EpiJSONReader reader(std::move(file));
        m_locations = reader.Read();
    }

    void MapController::setDay(const QString &day) {
        m_day = day.toInt();
        Update();
    }

    QString MapController::getDay() {
        return QString::number(m_day);
    }

    void MapController::initialize(QObject *root) {
        m_root = root;

        center();
        Update();
    }

    void MapController::Update() {
        QMetaObject::invokeMethod(m_root, "clear");

        for (auto const &location: m_locations) {
            QMetaObject::invokeMethod(m_root, "addLocation",
                                      Q_ARG(QVariant, location.id),
                                      Q_ARG(QVariant, location.latitude),
                                      Q_ARG(QVariant, location.longitude),
                                      Q_ARG(QVariant, location.size),
                                      Q_ARG(QVariant, location.infected[m_day])
            );
        }
    }

    void MapController::center(){
        auto ql = QList<QGeoCoordinate>();

        for (auto const &location: m_locations) {
            auto coor = new QGeoCoordinate(location.latitude, location.longitude);
            ql.append(*coor);
        }

        QGeoRectangle shape = QGeoRectangle(ql);

        QMetaObject::invokeMethod(m_root, "zoom", Q_ARG(QVariant, QVariant::fromValue(shape.boundingGeoRectangle())));
    }

    QString MapController::getNaam() {
        for (auto & loc: m_locations) {
            if (loc.id == m_id) {
                return QString::fromStdString(loc.name);
            }
        }
        std::cerr << m_id << std::endl;
    }

    QString MapController::getInfo() {
        for (auto & loc: m_locations){
            if(loc.id == m_id){
                string s =  "population: " + to_string(loc.size) + "\n" +
                            "infected: " + to_string(loc.infected[m_day]) + "\n" +
                            "longitude: " + to_string(loc.longitude) + "\n" +
                            "latitude: " + to_string(loc.latitude) + "\n";
                return QString::fromStdString(s);
            }
        }
    }

    void MapController::setID(const QString &id) {
        m_id = id.toInt();
    }

    QString MapController::getID(){
        return QString::number(m_id);
    }
}
