import QtQuick 2.12
import QtQuick.Window 2.12
import QtLocation 5.12
import QtPositioning 5.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Stride data visualization")

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map

        anchors.fill: parent
        anchors.bottomMargin: 30

        plugin: mapPlugin
        center: QtPositioning.coordinate(50.85, 4.35)
        zoomLevel: 10
    }

   function addLocation(locationId, latitude, longitude, radius) {
           var component = Qt.createComponent("mapcircle.qml");
           if (component.status === Component.Ready) {
           	   console.log("lol")
               var location = component.createObject(map);
               location.id = locationId;
               location.lat = latitude;
               location.longt = longitude;
               location.rad = radius;
               map.addMapItem(location);
           }
       }
}
