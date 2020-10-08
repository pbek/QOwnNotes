import QtQml 2.0
import QOwnNotesTypes 1.0
import QtWebSockets 1.1

/**
 * This is an example on how to use web sockets as a client
 * when sending data to an echo server
 * 
 * Under Ubuntu Linux I installed: qml-module-qtwebsockets
 * 
 * Also see: http://doc.qt.io/qt-5/qml-qtwebsockets-websocket.html
 */
WebSocket {
    id: socket
    url: "ws://echo.websocket.org"

    function init() {
        script.registerCustomAction("socketClientTest", "socketClientTest", "socketClientTest", "edit-copy");
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier == "socketClientTest") {
            socket.active = !socket.active
        }
    }

    onTextMessageReceived: {
        script.log("Received message: " + message);
    }
    onStatusChanged: if (socket.status == WebSocket.Error) {
                            script.log("Error: " + socket.errorString)
                        } else if (socket.status == WebSocket.Open) {
                            socket.sendTextMessage("Hello World")
                        } else if (socket.status == WebSocket.Closed) {
                            script.log("Socket closed");
                        }
    active: false
}
