#!/usr/bin/env node
var WebSocketClient = require('websocket').client;

var client = new WebSocketClient();
var count = 0
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
            profilingTime.clientReceiveTime = performance.now()*1000
            console.log(profilingTime);
        }

    });
    var appClientMessage =  {
        "type" : "application_client"
    }
    var groundLocationReset = {
        "type" : "application_control",
        "feature_id" : "ground_location",
        "action" : "reset" 
    }
    connection.send(JSON.stringify(appClientMessage));
});

client.connect('ws://localhost:8181/');