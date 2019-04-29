import QtQuick 2.9
import QtLocation 5.6
import QtPositioning 5.5

MapForm {
    Map{
        width: parent.width
        height: parent.height
        visible: true

        Plugin{
            id: mapPlugin
            name: "osm"
        }

        Map {
            anchors.fill: parent
            plugin: mapPlugin
            center: QtPositioning.coordinate(51.260197, 4.402771)
            zoomLevel: 7

            MapCircle {
                center {
                    latitude: 51.260197
                    longitude: 4.402771
                }
                radius: 200
                color: "green"
                opacity: 0.5
            }
        }
    }
}
