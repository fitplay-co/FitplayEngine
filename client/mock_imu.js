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
            console.log(message);
        }
    });
    var sensorClientMessage =  {
        "type" : "sensor_client",
        "sensor_id": "test_imu",
        "sensor_type": 'imu',
    }
    connection.send(JSON.stringify(sensorClientMessage));

    var sensorFrame =  {
        "type" : "sensor_frame",
        "sensor_id": "test_imu",
        "sensor_type": 'imu',
        "imu": {}
    }

    var count = 0
    setInterval(() => {
        sensorFrame.imu.count = count++
        connection.send(JSON.stringify(sensorFrame))
    }, 1000);
    
});

client.connect('ws://localhost:8181/');