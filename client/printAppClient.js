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
    var appClientMessage =  {
        "type" : "application_client",
        "id": "test_client"
    }
    var actionDetectionSubscribe = {
        "type" : "application_control",
        "feature_id" : "action_detection",
        "action" : "subsribe" 
    }
    var actionDetectionRelease = {
        "type" : "application_control",
        "feature_id" : "action_detection",
        "action" : "release" 
    }
    var groundLocationSubscribe = {
        "type" : "application_control",
        "feature_id" : "ground_loccation",
        "action" : "subsribe" 
    }
    var groundLocationRelease = {
        "type" : "application_control",
        "feature_id" : "ground_loccation",
        "action" : "release" 
    }
    var groundLocationReset = {
        "type" : "application_control",
        "feature_id" : "ground_location",
        "action" : "reset" 
    }
    connection.send(JSON.stringify(appClientMessage));
    connection.send(JSON.stringify(actionDetectionSubscribe))
    setTimeout(() => {
        connection.send(JSON.stringify(groundLocationSubscribe))
    }, 100)
    setTimeout(() => {
        connection.send(JSON.stringify(actionDetectionRelease))
    }, 10000);
});

client.connect('ws://localhost:8181/');