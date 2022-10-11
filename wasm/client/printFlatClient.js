#!/usr/bin/env node
var WebSocketClient = require('websocket').client;
var flatbuffers = require('flatbuffers');
var InputMessage = require('../../common/protocol/js/input/input-message').InputMessage
var OutputMessage = require('../../common/protocol/js/output/output-message').OutputMessage
var MessageType = require('../../common/protocol/js/input/message-type').MessageType
var Control = require('../../common/protocol/js/application-control/control').Control
var ControlData = require('../../common/protocol/js/application-control/control-data').ControlData
var Client = require('../../common/protocol/js/application-client/client').Client
var SensorClient = require('../../common/protocol/js/sensor/sensor-client').SensorClient
var SensorFrame = require('../../common/protocol/js/sensor/sensor-frame').SensorFrame
var SensorControl = require('../../common/protocol/js/sensor/sensor-control').SensorControl

let printFlat = 1;
let printPose = 1;

var fitplayModel = {
    "head_top" : 0,
    "thorax" : 1,
    "right_shoulder" : 12, //2
    "right_elbow" : 14, //3
    "right_wrist" : 16, //4
    "left_shoulder" : 11, //5
    "left_elbow" : 13, //6
    "left_wrist" : 15, //7
    "right_hip" : 24, //8
    "right_knee" : 26, //9
    "right_ankle" : 28, //10
    "left_hip" : 23, //11
    "left_knee" : 25, //12
    "left_ankle" : 27, //13
    "pelvis" : 14,
    "spine" : 15,
    "head" : 0, //16
    "right_hand" : 18, //17
    "left_hand" : 17, //18
    "right_toe" : 30, //19
    "left_toe" : 29 //20
}
// Object.keys(fitplayModel).forEach(function(key){
//     console.log(key, fitplayModel[key])
// })

var fitplayToMediapipeLandmarkIndexMap = new Array();
fitplayToMediapipeLandmarkIndexMap = [
    16, // nose[0]
    16, // left_eye_inner[1]
    16, // left_eye[2]
    16, // left_eye_outer[3]
    16, // right_eye_inner[4]
    16, // right_eye[5]
    16, // right_eye_outer[6]
    16, // left_ear[7]
    16, // right_ear[8]
    16, // mouth_left[9]
    16, // mouth_right[10]
    5, // left_shoulder[11]
    2, // right_shoulder[12]
    6, // left_elbow[13]
    3, // right_elbow[14]
    7, // left_wrist[15]
    4, // right_wrist[16]
    18, // left_pinky[17]
    17, // right_pinky[18]
    18, // left_index[19]
    17, // right_index[20]
    18, // left_thumb[21]
    17, // right_thumb[22]
    11, // left_hip[23]
    8, // right_hip[24]
    12, // left_knee[25]
    9, // right_knee[26]
    13, // left_ankle[27]
    10, // right_ankle[28]
    20, // left_heel[29]
    19, // right_heel[30]
    20, // left_foot_index[31]
    19 // right_foot_index[32]
]


var client = new WebSocketClient();
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
            if(printPose == 1)
            {if(JSON.parse(message.utf8Data).action_detection)
            {var rr = JSON.parse(message.utf8Data).action_detection.walk;
            console.log(rr.leftLeg + "," + rr.rightLeg + "," + rr.leftFrequency + "," + rr.rightFrequency + "," + rr.leftHipAng
                    + "," + rr.rightHipAng + "," + rr.leftStepLength + "," + rr.rightStepLength + "," + rr.turn + "," +
                    rr.stepRate + "," + rr.stepLen + "," + rr.velocity + "," + rr.velocityThreshold + "," + rr.realtimeLeftLeg
                    + "," + rr.realtimeRightLeg);}}
            else 
            {console.log("Frame")
            console.log("keyPoints")
            JSON.parse(message.utf8Data).pose_landmark.keypoints.forEach(element => {
                console.log(element.name + "," +element.x + "," + element.y + "," + element.z )
            });
            console.log("keyPoints3D")
            JSON.parse(message.utf8Data).pose_landmark.keypoints3D.forEach(element => {
                console.log(element.name + "," +element.x + "," + element.y + "," + element.z )
            });}
            //console.log(JSON.parse(message.utf8Data))
        } else {
            // console.log(message.binaryData)
            var outputMessage = OutputMessage.getRootAsOutputMessage(new flatbuffers.ByteBuffer(message.binaryData))
            var detectionResult = outputMessage.detectionResult()
            var pose = outputMessage.pose()
            // console.log("legLength:"+detectionResult.walk().leftLeg())
            // console.log(outputMessage.type() + "|" + outputMessage.sensorType())
            console.log("Frame")
            Object.keys(fitplayModel).forEach(function(key){
                var num = fitplayModel[key]
                console.log(key + "," + pose.keypoints(num).x() + "," + pose.keypoints(num).y() + "," + pose.keypoints(num).z()
                + "," + pose.keypoints(num).score())
            })
        }
    });
    if(printFlat == 1)
    {var appClientMessage =  {
        "type" : "application_client",
        "id": "test_client"
        // "useJson": true
    }}
    else {
        var appClientMessage =  {
            "type" : "application_client",
            "id": "test_client",
            "useJson": true
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

    connection.send(Buffer.from(flatbuffersMessage(appClientMessage)));
    connection.send(Buffer.from(flatbuffersMessage(actionDetectionSubscribe)))
    connection.send(Buffer.from(flatbuffersMessage(groundLocationSubscribe)))
    connection.send(Buffer.from(flatbuffersMessage(gazeSubscribe)))
    connection.send(Buffer.from(flatbuffersMessage(fittingSubscribe)))
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
        Client.addUseJson(builder, message.useJson)
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