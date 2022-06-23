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
            // var rr = JSON.parse(message.utf8Data).timeProfiling;
            // console.log(rr)
            // console.log("Frame")
            // console.log("keyPoints")
            // JSON.parse(message.utf8Data).pose_landmark.keypoints.forEach(element => {
            //     console.log(element.name + "," +element.x + "," + element.y + "," + element.z )
            // });
            // console.log("keyPoints3D")
            // JSON.parse(message.utf8Data).pose_landmark.keypoints3D.forEach(element => {
            //     console.log(element.name + "," +element.x + "," + element.y + "," + element.z )
            // });
        }
    });
    var appClientMessage =  {
        "type" : "application_client",
        "id": "test_client"
    }
    var imuFPS = {
        "type" : "application_control",
        "feature_id" : "imu",
        "action" : "config",
        "data": {
            fps: 1
        }
    }
    var actionDetectionSubscribe = {
        "type" : "application_control",
        "feature_id" : "action_detection",
        "action" : "subscribe" 
    }
    var actionDetectionRelease = {
        "type" : "application_control",
        "feature_id" : "action_detection",
        "action" : "release" 
    }
    var groundLocationSubscribe = {
        "type" : "application_control",
        "feature_id" : "ground_location",
        "action" : "subscribe" 
    }
    var groundLocationRelease = {
        "type" : "application_control",
        "feature_id" : "ground_location",
        "action" : "release" 
    }
    var groundLocationReset = {
        "type" : "application_control",
        "feature_id" : "ground_location",
        "action" : "reset" 
    }
    var groundLocationReset = {
        "type" : "application_control",
        "feature_id" : "ground_location",
        "action" : "reset" 
    }
    connection.send(JSON.stringify(appClientMessage));
    connection.send(JSON.stringify(imuFPS));
    connection.send(JSON.stringify(actionDetectionSubscribe))
    connection.send(JSON.stringify(groundLocationSubscribe))
    setTimeout(() => {
        connection.send(JSON.stringify(actionDetectionRelease))
    }, 10000);
});

client.connect('ws://localhost:8181/');