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
        anchors.right: sliderBar.left
        anchors.bottom: bottomBar.top
        anchors.left: parent.left
        anchors.top: parent.top

        plugin: mapPlugin
        zoomLevel: 10

        gesture.enabled: true

        property var circles: []
        anchors.bottomMargin: 0

        MouseArea {
            id: mArea
            anchors.fill: parent

            property var coor;
            property variant selectionCircle : QtPositioning.circle()
            property variant selectionRectangle : QtPositioning.rectangle()

            anchors.bottomMargin: 0

            hoverEnabled: false

            onPressed: {
                if(!map.gesture.enabled){
                    coor = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                    if (button2.checked){
                        selectRect.topLeft = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        selectRect.bottomRight = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        selectionRectangle.type = GeoShape.RectangleType;
                        selectionRectangle.bottomRight = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        selectionRectangle.topLeft = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        selectRect.visible = true;
                    }
                    else{
                        selectCircle.center = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        selectionCircle.type = GeoShape.CircleType;
                        selectionCircle.center = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        selectCircle.radius = 0;
                        selectCircle.visible = true;
                    }
                }
            }
            onPositionChanged: {
                if (!map.gesture.enabled){
                    if(button2.checked){
                        if(map.toCoordinate(Qt.point(mouse.x, mouse.y)).longitude > coor.longitude){
                            selectRect.topLeft = coor
                            selectRect.bottomRight = map.toCoordinate(Qt.point(mouse.x,mouse.y))
                            selectionRectangle.topLeft = coor
                            selectionRectangle.bottomRight = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        }else{
                            selectRect.bottomRight = coor
                            selectRect.topLeft = map.toCoordinate(Qt.point(mouse.x,mouse.y))
                            selectionRectangle.bottomRight = coor
                            selectionRectangle.topLeft = map.toCoordinate(Qt.point(mouse.x,mouse.y));
                        }


                    }
                    else{
                        selectCircle.radius = coor.distanceTo(map.toCoordinate(Qt.point(mouse.x,mouse.y)));
                        selectionCircle.radius = coor.distanceTo(map.toCoordinate(Qt.point(mouse.x,mouse.y)));
                    }
                }
            }
            onReleased: {
                if(button2.checked){
                    controller.rectangle = selectionRectangle
                } else {
                    controller.circle = selectionCircle
                }
            }
        }

        MapCircle {
            id: selectCircle
            color: "chartreuse"
            border.width: 3
            opacity: 0.5
            visible: false

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    controller.ID = "circle"
                    controller.setData()
                }
            }
        }

        MapRectangle {
            id: selectRect
            color: "chartreuse"
            border.width: 3
            opacity: 0.5
            visible: false

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    controller.ID = "rectangle"
                    controller.setData()
                }
            }
        }
    }

    Rectangle {
        id: bottomBar
        y: 410
        height: 70
        color: "#ffffff"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: sliderBar.left
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

        Button {
            id: button2
            x: 16
            y: 22
            text: qsTr("select Rectangle")
            autoRepeat: false
            checkable: true
            anchors.verticalCenter: parent.verticalCenter

            onCheckedChanged: {
                if(checked){
                    button3.checked = false
                }
                map.gesture.enabled = !checked
            }
        }

        Button {
            id: button3
            y: 22
            text: qsTr("select Circle")
            anchors.left: button2.right
            anchors.leftMargin: 20
            checkable: true
            anchors.verticalCenter: parent.verticalCenter

            onCheckedChanged: {
                if(checked){
                    button2.checked = false
                }
                map.gesture.enabled = !checked
            }
        }

        Button {
            id: button4
            y: 13
            text: qsTr("Clear")
            anchors.left: button3.right
            anchors.leftMargin: 20
            anchors.verticalCenterOffset: 0
            checkable: false
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                selectCircle.visible = false;
                selectRect.visible = false;
                dataBar.visible = false;
            }
        }
    }

    Rectangle {
        id: sliderBar
        x: 507
        y: 0
        width: 133
        color: "#ffffff"
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
            y: 23
            color: "#000000"
            text: qsTr("0")
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            elide: Text.ElideRight
            font.pixelSize: 19
        }
    }

    Rectangle {
        id: dataBar
        x: 380
        visible: false
        width: Math.min(map.width/4, 500)
        color: "#ffffff"
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: sliderBar.left
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        Button {
            id: closeButton
            text: "X"
            width: 30
            height: 30

            anchors.top: dataBar.top
            anchors.right: dataBar.right
            anchors.topMargin: 10
            anchors.rightMargin: 20

            onClicked: {
                dataBar.visible = false
                controller.ID = "-1"
            }
        }

        ListView {
            id: list
            boundsBehavior: Flickable.StopAtBounds
            anchors.topMargin: 79
            anchors.fill: parent
            anchors.margins: 20

            model: model

            delegate: statusDelegate

            section.delegate: ageDelegate
            section.property: "ageBracket"
        }


        Rectangle {
            id: rectangle
            width: 30
            height: 50
            color: "#ffffff"
            anchors.rightMargin: 0
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.topMargin: 0
        }

        Text {
            id: name
            x: 76
            y: 51
            width: 86
            height: 30
            text: qsTr("Text")
            anchors.horizontalCenter: parent.horizontalCenter
            z: 5
            anchors.verticalCenter: rectangle.verticalCenter
            fontSizeMode: Text.Fit
            wrapMode: Text.WordWrap
            font.underline: true
            font.bold: true
            font.pixelSize: 20
        }

        Component {
            id: statusDelegate

            Text {
                width: ListView.view.width
                height: 40
                text: status
            }
        }

        Component {
            id: ageDelegate

            Rectangle {
                width: ListView.view.width
                height: 40
                radius: 5
                color: "white"

                border.width: 1
                border.color: "#696969"

                Text {
                    horizontalAlignment: Text.AlignHCenter

                    text: "<b>" + section + "</b>"

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        ListModel{
            id: model
        }


    }



    function addLocation(locationId, latitude, longitude, pop, infected) {
        var component = Qt.createComponent("mapcircle.qml");
        if (component.status === Component.Ready) {
            var location = component.createObject(map);
            location.lat = latitude;
            location.dataId = locationId;
            location.longt = longitude;
            location.rad = pop;
            location.infected = infected;
            map.circles.push(location)
            map.addMapItem(location);
        }
    }

    function clear(){
        while(map.circles.length > 0){
            map.removeMapItem(map.circles[0]);
            map.circles[0].destroy();
            map.circles.shift();
        }
    }

    function zoom(geo){
        map.visibleRegion = geo
    }

    function setInfo(epiData){
        var ages = ["College", "Daycare", "Household", "K12School", "PreSchool", "PrimaryCommunity", "SecondaryCommunity", "Workplace"];
        var healts = ["immune", "infected", "infectious", "recovered", "susceptible", "symptomatic", "total"];

        model.clear()

        name.text = controller.naam;

        for(var i = 0; i < ages.length; i++){
            for(var j = 0; j < healts.length; j++){
                model.append({ status: healts[j] + ": " + epiData[ages[i]][healts[j]], ageBracket: ages[i] })
            }
        }
        dataBar.visible = true;
    }

    function setDays(days){
        slider.to = days;
    }
}



/*##^## Designer {
    D{i:19;anchors_height:50}D{i:20;anchors_width:86;anchors_x:76}
}
 ##^##*/
