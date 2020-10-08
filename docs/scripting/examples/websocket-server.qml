import QtQml 2.0
import QOwnNotesTypes 1.0
import QtWebSockets 1.1

/**
 * This is an example on how to use web sockets as socket server
 * This can be for example used to remote control QOwnNotes
 * 
 * You can test the socket server by connecting to it on
 * https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345
 * 
 * Under Ubuntu Linux I installed: qml-module-qtwebsockets
 * 
 * Also see: http://doc.qt.io/qt-5/qml-qtwebsockets-websocketserver.html
 */
WebSocketServer {
    // if you don't set a port then a random port will be chosen
    port: 35345 

    id: server
    listen: true

    function init() {
        script.registerCustomAction("socketServerTest", "Print socket server url", "socketServerTest", "edit-copy");
        script.log("Socket server url: " + server.url);
    }

    /**
     * This function is invoked when a custom action is triggered
     * in the menu or via button
     * 
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        if (identifier == "socketServerTest") {
            script.log("Socket server url: " + server.url);
        }
    }
    
    onClientConnected: {
        webSocket.onTextMessageReceived.connect(function(message) {
            script.log(qsTr("Server received message: %1").arg(message));
            webSocket.sendTextMessage(qsTr("Hello Client!"));
        });
    }
    
    onErrorStringChanged: {
        script.log(qsTr("Server error: %1").arg(errorString));
    }
}
