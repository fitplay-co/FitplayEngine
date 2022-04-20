#!/usr/bin/env node
var WebSocketClient = require('websocket').client;

var client = new WebSocketClient();

client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(connection) {
    console.log('WebSocket Client Connected');
    connection.on('error', function(error) {
        console.log("Connection Error: " + error.toString());
    });
    connection.on('close', function() {
        console.log('echo-protocol Connection Closed');
    });
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            var profilingTime = JSON.parse(message.utf8Data).timeProfiling 
            profilingTime.clientReceiveTime = Date.now()
            console.log(profilingTime);
        }
    });
    var appClientMessage =  {
        "type" : "application_client"
    }
    connection.send(JSON.stringify(appClientMessage));
});

client.connect('ws://localhost:8181/');