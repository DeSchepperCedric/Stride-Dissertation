    import QtQuick 2.0
import QtLocation 5.12

import QtQuick.Window 2.12
import QtQuick.Controls 2.12




MapCircle {
    property real lat
    property real longt
    property real rad
    property string dataId
    property real infected

    id: dataId
    radius: rad*0.1
    color: Qt.hsva(getColor(infected/rad)/360,1, 0.5, 0.5)
    border.width: 3

    center {
        latitude: lat
        longitude: longt
    }

    Dialog{
        id: dia
        visible: false

        width: 300
        height: 150

        title: "name"

        Text{
            id: info
            text: "info"
        }
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        /*onEntered: {
            controller.ID = parent.dataId
            dia.title = controller.naam
            info.text = controller.info
            dia.visible = true;
        }

        onExited: {
            dia.visible = false
        }*/

        onClicked: {
            console.log(parent.dataId)
            controller.ID = parent.dataId
            controller.setData()
        }
    }

    function getColor(value){
        var hue=((1-value*10)*120);
        return hue;
    }
 }

