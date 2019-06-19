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
#include <geopop/io/EpiReaderFactory.h>
#include <geopop/io/EpiReader.h>

#include <geopop/EnhancedCoordinate.h>

#include <unistd.h>

namespace visualization {

    using namespace std;


    MapController::MapController(const std::string &file_name) : QObject(nullptr), m_circle(), m_rect(), m_locations(), m_grid(), m_coords() {
        geopop::EpiReaderFactory fac;
        shared_ptr<geopop::EpiReader> reader =
                fac.CreateReader(file_name);
        auto r = reader->Read();
        m_locations = r.first;
        m_coords = r.second;
        for(auto& loc: m_coords){
            auto temp = make_shared<geopop::EnhancedCoordinate>(new geopop::EnhancedCoordinate(nullptr));
            temp->setData(loc.getData<void>());
            temp->SetCoordinate(loc.GetCoordinate());
            m_grid.AddData(temp);

        }
        m_grid.Finalize();
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

        if(!m_coords.empty()){
            int size = m_coords[0].getData<Location>()->infected["College"]["total"].size();
            QMetaObject::invokeMethod(m_root, "setDays", Q_ARG(QVariant, size-1));
        }

        Update();
        center();
    }

    void MapController::Update() {
        QMetaObject::invokeMethod(m_root, "clear");

        for (auto &location: m_coords) {
            auto coor = location.GetCoordinate();
            QMetaObject::invokeMethod(m_root, "addLocation",
                                      Q_ARG(QVariant, location.getData<visualization::Location>()->id),
                                      Q_ARG(QVariant, coor.get<0>()),
                                      Q_ARG(QVariant, coor.get<1>()),
                                      Q_ARG(QVariant, location.getData<visualization::Location>()->size),
                                      Q_ARG(QVariant, location.getData<visualization::Location>()->infected["College"]["total"][m_day])
            );
        }
        setData();
    }

    void MapController::center(){
        auto ql = QList<QGeoCoordinate>();

        for (auto const &location: m_coords) {
            auto coord = location.GetCoordinate();
            auto coor = new QGeoCoordinate(coord.get<0>(), coord.get<1>());
            ql.append(*coor);
        }

        QGeoRectangle shape = QGeoRectangle(ql);

        QMetaObject::invokeMethod(m_root, "zoom", Q_ARG(QVariant, QVariant::fromValue(shape.boundingGeoRectangle())));
    }

    QString MapController::getNaam() {
        if(m_id == "rectangle"){
            return QString::fromStdString(m_id);
        }else if(m_id == "circle"){
            return QString::fromStdString(m_id);
        }else{
            for (auto & loc: m_locations) {
                if (to_string(loc->id) == m_id) {
                    return QString::fromStdString(loc->name);
                }
            }
        }
        std::cerr << m_id << std::endl;
        return QString::fromStdString("");
    }

    QString MapController::getInfo() {
        for (auto & loc: m_coords){
            if(to_string(loc.getData<visualization::Location>()->id) == m_id){
                auto coord = loc.GetCoordinate();
                string s =  "population: " + to_string(loc.getData<visualization::Location>()->size) + "\n" +
                            "longitude: " + to_string(coord.get<0>()) + "\n" +
                            "latitude: " + to_string(coord.get<0>()) + "\n";
                return QString::fromStdString(s);
            }
        }
        return QString::fromStdString("");
    }

    void MapController::setID(const QString &id) {
        m_id = id.toStdString();
    }

    QString MapController::getID(){
        return QString::fromStdString(m_id);
    }

    void MapController::setData() {

        const auto age = {"College", "Daycare", "Household", "K12School", "PreSchool", "PrimaryCommunity", "SecondaryCommunity", "Workplace"};
        const auto status = {"immune", "infected", "infectious", "recovered", "susceptible", "symptomatic", "total"};

        std::unordered_map<string, unordered_map<string, unsigned int>> epi = {{{}}};
        bool found = false;

        if(m_id == "rectangle"){
            geopop::Coordinate right(m_rect.topLeft().latitude(), m_rect.topLeft().longitude());
            geopop::Coordinate left(m_rect.bottomRight().latitude(), m_rect.bottomRight().longitude());
            geopop::EnhancedCoordinate top(nullptr, right);
            geopop::EnhancedCoordinate bottom(nullptr, left);
            auto list = m_grid.LocationsInBox(&top, &bottom);
            if(!list.empty()){
                found = true;
                for (auto &a: age) {
                    for (auto &s: status) {
                        epi[a][s] = 0;
                    }
                }
                for(auto i : list){
                    for (auto &a: age) {
                        for (auto &s: status) {
                            auto temp = *i->getData<Location>();
                            epi[a][s] += i->getData<Location>()->infected[a][s][m_day];
                        }
                    }
                }
            }
        }else if(m_id == "circle"){
            geopop::Coordinate center(m_circle.center().latitude(), m_circle.center().longitude());
            geopop::EnhancedCoordinate coord(nullptr, center);

            auto list = m_grid.LocationsInRadius(coord, m_circle.radius()/1000);

            if(!list.empty()){
                found = true;
                for (auto &a: age) {
                    for (auto &s: status) {
                        epi[a][s] = 0;
                    }
                }
                for(unsigned int i = 0; i < list.size(); i++){
                    for (auto &a: age) {
                        for (auto &s: status) {
                            epi[a][s] += list[i]->getData<Location>()->infected[a][s][m_day];
                        }
                    }
                }
            }

        }else{
            for(auto loc: m_locations){
                if(to_string(loc->id) == m_id){
                    found = true;

                    for (auto &a: age) {
                        for (auto &s: status) {
                            epi[a][s] = loc->infected[a][s][m_day];
                        }
                    }
                }
            }
        }

        if(found) {
            QVariantMap data;
            for (auto &a: age) {
                QVariantMap health;
                for (auto &s: status) {
                    health[s] = QVariant(epi[a][s]);
                }
                data[a] = health;
            }
            QMetaObject::invokeMethod(m_root, "setInfo",
                                      Q_ARG(QVariant, QVariant::fromValue(data))

            );
        }
    }
}
