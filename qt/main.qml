import QtQuick 2.12
import QtQuick.Window 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick.Controls 2.3

Window {
    id: window
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
        anchors.right: rectangle.left
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.rightMargin: 0

        anchors.bottomMargin: 0

        plugin: mapPlugin
        zoomLevel: 10
        }

    Rectangle {
        id: rectangle
        x: 507
        y: 0
        width: 133
        color: "#c81212"
        z: 1
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#c81212"
            }

            GradientStop {
                position: 1
                color: "#000000"
            }
        }
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0

        Slider {
            id: slider
            x: 8
            width: 100
            anchors.top: element.bottom
            anchors.topMargin: 32
            anchors.bottom: button.top
            anchors.bottomMargin: 24
            orientation: Qt.Vertical
            snapMode: Slider.SnapOnRelease
            to: 50
            stepSize: 1
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            value: 0
            onValueChanged: {
                controller.day = value
                element.text = value
            }
        }

        Button {
            id: button
            x: 17
            y: 419
            text: qsTr("center")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: controller.center()
        }

        Text {
            id: element
            x: 44
            y: 93
            color: "#ffffff"
            text: qsTr("0")
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            elide: Text.ElideRight
            font.pixelSize: 19
        }
    }


    function addLocation(locationId, latitude, longitude, pop, infected) {
        var component = Qt.createComponent("mapcircle.qml");
        if (component.status === Component.Ready) {
            var location = component.createObject(map);
            location.id = locationId;
            location.lat = latitude;
            location.longt = longitude;
            location.rad = pop;
            location.infected = infected;
            map.addMapItem(location);
        }
    }

    function clear(){
        map.clearMapItems()
    }

    function zoom(geo){
        map.visibleRegion = geo
    }


}
