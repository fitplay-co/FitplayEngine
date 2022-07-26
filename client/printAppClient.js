#!/usr/bin/env node
var WebSocketClient = require('websocket').client;
var flatbuffers = require('flatbuffers');
var InputMessage = require('../protocol/js/input/input-message').InputMessage
var MessageType = require('../protocol/js/input/message-type').MessageType
var Control = require('../protocol/js/application-control/control').Control
var ControlData = require('../protocol/js/application-control/control-data').ControlData
var Client = require('../protocol/js/application-client/client').Client
var SensorClient = require('../protocol/js/sensor/sensor-client').SensorClient
var SensorFrame = require('../protocol/js/sensor/sensor-frame').SensorFrame
var SensorControl = require('../protocol/js/sensor/sensor-control').SensorControl

var client = new WebSocketClient();
var count = 0
client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(connection) {
    console.log('WebSocket Client Connected');
    connection.binaryType = "arraybuffer"
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
            console.log(JSON.parse(message.utf8Data))
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
    var gazeSubscribe = {
        "type" : "application_control",
        "feature_id" : "gaze_tracking",
        "action" : "subscribe" 
    }
    var fittingSubscribe = {
        "type" : "application_control",
        "feature_id" : "fitting",
        "action" : "subscribe" 
    }
    var heartControlSubscribe = {
        "type" : "sensor_control",
        "feature_id" : "heart_control",
        "command" : "3"
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

    connection.send(Buffer.from(flatbuffersMessage(appClientMessage)));
    connection.send(Buffer.from(flatbuffersMessage(imuFPS)));
    connection.send(Buffer.from(flatbuffersMessage(actionDetectionSubscribe)))
    connection.send(Buffer.from(flatbuffersMessage(groundLocationSubscribe)))
    connection.send(Buffer.from(flatbuffersMessage(gazeSubscribe)))
    connection.send(Buffer.from(flatbuffersMessage(fittingSubscribe)))
    // connection.send(JSON.stringify(heartControlSubscribe))
    setTimeout(() => {
        connection.send(Buffer.from(flatbuffersMessage(actionDetectionRelease)))
    }, 10000);

    // connection.send(JSON.stringify(flatbuffersMessage(appClientMessage));
    // connection.send(JSON.stringify(imuFPS));
    // connection.send(JSON.stringify(actionDetectionSubscribe))
    // connection.send(JSON.stringify(groundLocationSubscribe))
    // connection.send(JSON.stringify(gazeSubscribe))
    // connection.send(JSON.stringify(fittingSubscribe))
    // // connection.send(JSON.stringify(heartControlSubscribe))
    // setTimeout(() => {
    //     connection.send(JSON.stringify(actionDetectionRelease))
    // }, 10000);
});

function flatbuffersMessage(message) {
    type = message.type
    if(type === 'pose_landmark') {
        const pose = message
        var builder = new flatbuffers.Builder(1024)     
        var actionName = builder.createString("aciton_detection, gaze_tracking, ground_location, fitting")   
        var keyPoints = new Array(33)
        for (var i = 0;i<33;i++){
            var name = builder.createString(pose.keypoints[i].name)
            Point.startPoint(builder)
            Point.addX(builder,pose.keypoints[i].x)
            Point.addY(builder,pose.keypoints[i].y)
            Point.addZ(builder,pose.keypoints[i].z)
            Point.addScore(builder,pose.keypoints[i].score)
            Point.addName(builder,name)
            var point = Point.endPoint(builder)
            //console.log(point)
            keyPoints[i] = point
        }
        //console.log(keyPoints)
        var KeyPoints = Pose.createKeypointsVector(builder,keyPoints)

        var keypoints3d = new Array(33)
        for (var i = 0;i<33;i++){
            var name = builder.createString(pose.keypoints3D[i].name)
            Point.startPoint(builder)
            Point.addX(builder,pose.keypoints3D[i].x)
            Point.addY(builder,pose.keypoints3D[i].y)
            Point.addZ(builder,pose.keypoints3D[i].z)
            Point.addScore(builder,pose.keypoints3D[i].score)
            Point.addName(builder,name)
            var point = Point.endPoint(builder)
            //console.log(point)
            keypoints3d[i] = point
        }
        //console.log(KeyPoints)
        var keyPoints3d = Pose.createKeypoints3DVector(builder,keypoints3d)

        Pose.startPose(builder)
        Pose.addAction(builder, actionName)
        Pose.addRgbdEnabled(builder, pose.rgbdEnabled)
        Pose.addKeypoints(builder,KeyPoints)
        Pose.addKeypoints3D(builder,keyPoints3d)
        var poseOffset = Pose.endPose(builder)

        var version = builder.createString('0.1.0')
        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.Pose)
        InputMessage.addPose(builder, poseOffset)
        builder.finish(InputMessage.endInputMessage(builder))
        return builder.asUint8Array()
    } else if (type === 'application_control') {
        var builder = new flatbuffers.Builder(1024)
        var version = builder.createString('0.1.0')
        var featureId = builder.createString(message.feature_id)
        var action = builder.createString(message.action)
        var controlDataOffset
        if (message.data) {
            ControlData.startControlData(builder)
            ControlData.addFps(builder, message.data.fps)
            controlDataOffset = ControlData.endControlData(builder)
        }
        Control.startControl(builder)
        Control.addAction(builder, action)
        Control.addFeatureId(builder, featureId)
        if (controlDataOffset) {
            Control.addData(builder, controlDataOffset)
        }
        var sensorControlOffset = Control.endControl(builder)

        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.ApplicationControl)
        InputMessage.addControl(builder, sensorControlOffset)
        builder.finish(InputMessage.endInputMessage(builder))
        return builder.asUint8Array()
    } else if (type === 'application_client') {
        var builder = new flatbuffers.Builder(1024)
        var version = builder.createString('0.1.0')
        var id = builder.createString(message.id)
        Client.startClient(builder)
        Client.addId(builder, id)
        var sensorControlOffset = Client.endClient(builder)

        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.ApplicationClient)
        InputMessage.addClient(builder, sensorControlOffset)
        builder.finish(InputMessage.endInputMessage(builder))
        return builder.asUint8Array()
    } else if (type === 'sensor_client') {
        var builder = new flatbuffers.Builder(1024)
        var version = builder.createString('0.1.0')
        var sensorId = builder.createString(message.sensor_id)
        var sensorType = builder.createString(message.sensor_type)
        var sensorInfo = builder.createString(message.sensor_info)
        SensorClient.startSensorClient(builder)
        SensorClient.addSensorId(builder, sensorId)
        SensorClient.addSensorType(builder, sensorType)
        SensorClient.addSensorInfo(builder, sensorInfo)
        var sensorControlOffset = SensorClient.endSensorClient(builder)

        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.SensorClient)
        InputMessage.addSensorClient(builder, sensorControlOffset)
        builder.finish(InputMessage.endInputMessage(builder))
        return builder.asUint8Array()
    } else if (type === 'sensor_frame') {
        var builder = new flatbuffers.Builder(1024)
        var version = builder.createString('0.1.0')
        var sensorId = builder.createString(message.sensor_id)
        var sensorType = builder.createString(message.sensor_type)
        SensorFrame.startSensorFrame(builder)
        SensorFrame.addSensorId(builder, sensorId)
        SensorFrame.addSensorType(builder, sensorType)
        var sensorControlOffset = SensorFrame.endSensorFrame(builder)

        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.SensorFrame)
        InputMessage.addSensorFrame(builder, sensorControlOffset)
        builder.finish(InputMessage.endInputMessage(builder))
        return builder.asUint8Array()
    } else if (type === 'sensor_control') {
        var builder = new flatbuffers.Builder(1024)
        var version = builder.createString('0.1.0')
        var sensorId = builder.createString(message.sensor_id)
        var sensorType = builder.createString(message.sensor_type)
        SensorControl.startSensorControl(builder)
        SensorControl.addSensorId(builder, sensorId)
        SensorControl.addSensorType(builder, sensorType)
        var sensorControlOffset = SensorControl.endSensorControl(builder)

        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.SensorControl)
        InputMessage.addSensorControl(builder, sensorControlOffset)
        builder.finish(InputMessage.endInputMessage(builder))
        return builder.asUint8Array()
    }
}

client.connect('ws://localhost:8181/');