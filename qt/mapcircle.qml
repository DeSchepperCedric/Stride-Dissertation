import QtQuick 2.0
import QtLocation 5.12

MapCircle {
    property real lat
    property real longt
    property real rad
    property color color
    property string id

    id: location
    radius: rad
    color: 'green'
    border.width: 3

    center {
                latitude: lat
                longitude: longt
            }
}
