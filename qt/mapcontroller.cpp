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

namespace visualization {

MapController::MapController(const std::string &file_name): QObject (nullptr){
    std::string temp = file_name;
    auto file = std::make_unique<std::ifstream>();
    file->open("/home/wannes/Desktop/UA/3e_bach/BP/Stride-Dissertation/temp.json");
    geopop::EpiJSONReader reader(std::move(file));
    m_locations = reader.Read();
}

void MapController::setDay(const QString &day){
    auto temp = static_cast<int>(day.toInt());
    m_day = temp;
}

QString MapController::getDay(){
    return QString::number(m_day);
}

void MapController::initialize(QObject* root){
    m_root = root;
    for(auto const &location: m_locations){
        std::cout << location.size << std::endl;
        QMetaObject::invokeMethod(m_root, "addLocation", Q_ARG(QVariant, QString::number(location.id)),
                                                  Q_ARG(QVariant, QVariant::fromValue(location.latitude)),
                                                  Q_ARG(QVariant, QVariant::fromValue(location.longitude)),
                                                  Q_ARG(QVariant, QVariant::fromValue(location.size * 0.1)));
    }
}

}
