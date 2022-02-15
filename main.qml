import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Server App")

    Connections {
        target: server
        function onBroadcast(ba) {
            listModelMessages.append({
                                        message: ba + ""
                                     })
        }
    }

    ColumnLayout {
        anchors.fill: parent
        ListView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true
            model: ListModel {
                id: listModelMessages
                ListElement {
                    message: "Welcome to server"
                }
            }
            delegate: ItemDelegate {
                text: message
            }
            ScrollBar.vertical: ScrollBar {}
        }
        RowLayout {
            TextField {
                id: textFieldMessage
                placeholderText: qsTr("Your message...")
                color: "black"
                Layout.fillWidth: true
                onAccepted: buttonSend.clicked()
            }
            Button {
                id: buttonSend
                text: qsTr("Send")
                palette.buttonText: "black"
                onClicked: {
                    server.sendBroadcast(textFieldMessage.text)
                    textFieldMessage.clear()
                }
            }
        }
    }

}
