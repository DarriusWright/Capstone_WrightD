import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Rectangle {
    id: root
    color: "red"
    width: 200
    height: 200

	signal changeModelName()

    Text {
        id: text
        text: "This is QML code.\n(Click to pause)"
        font.pointSize: 14
        anchors.centerIn: parent
        PropertyAnimation {
            id: animation
            target: text
            property: "rotation"
            from: 0; to: 360; duration: 5000
            loops: Animation.Infinite
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: animation.paused ? animation.resume() : animation.pause()
    }

	Button{
		text: "Click Me"
		onClicked : root.changeModelName();
	}

    Component.onCompleted: animation.start()
}