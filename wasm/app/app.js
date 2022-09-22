var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
const wasm = require('./src/midware_wasm');
//const { type } = require('os');
const {performance} = require('perf_hooks');
const messageBuffer = require('./src/message_buffer');
const childProcess = require('child_process');

var flatbuffers = require('flatbuffers');
var Point = require('../../common/protocol/js/pose-data/point').Point
var Pose = require('../../common/protocol/js/pose-data/pose').Pose;
var InputMessage = require('../../common/protocol/js/input/input-message').InputMessage
var MessageType = require('../../common/protocol/js/input/message-type').MessageType
var Control = require('../../common/protocol/js/application-control/control').Control
var ControlData = require('../../common/protocol/js/application-control/control-data').ControlData
var Client = require('../../common/protocol/js/application-client/client').Client
var SensorClient = require('../../common/protocol/js/sensor/sensor-client').SensorClient
var SensorFrame = require('../../common/protocol/js/sensor/sensor-frame').SensorFrame
var SensorControl = require('../../common/protocol/js/sensor/sensor-control').SensorControl

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/users', usersRouter);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;

const ADVANCED_FEATURES = ['ground_location', 'action_detection', 'gaze_tracking', 'throw_direction', 'fitting']

//start server app 
var WebSocketServer = require('ws').Server
var activeApplicationClient = []
var clientMap = new Map() // id -> client映射
var clientIdMap = new Map() // client -> id映射
var clientSubscriptionMap = new Map() // client订阅的高级能力
var times = [];
var counter = 0
var processingJob = 0

// 创建消息处理循环
var messageLoop = coroutine(function*() {
    var bufferData;
    while (bufferData = yield) { // 等待下一个消息
        const message = bufferData.message
        const ws = bufferData.webSocket
        const rawData = bufferData.rawData
        type = message.type()

        messageTime = Math.round(performance.now()*1000)+'μs'
        processingTime = Math.round(performance.now()*1000)+'μs'
        // 中间件处理
        wasm.process(rawData, bufferData.jsonMessage)
        // os处理
        if(type === MessageType.Pose) {
            pose = bufferData.jsonMessage
      
            pose.timeProfiling = {}
            pose.timeProfiling.serverReceive = messageTime
            pose.timeProfiling.processingTime = processingTime

            pose.sensor_type = "camera"

            //调整pose结构适配api格式
            pose.type = "application_frame"
            pose.pose_landmark = {
                keypoints: pose.keypoints,
                keypoints3D: pose.keypoints3D,
                timestamp: Date.now(),
                version:"0.1.0"
            }
            
            delete pose.keypoints
            delete pose.keypoints3D

            pose.timeProfiling.beforeSendTime = Math.round(performance.now()*1000)+'μs'

            activeApplicationClient.forEach(function(ws){
                if(!ws.notActived) {
                    const poseDataForClient = Object.assign({}, pose)
                    if ('ground_location' in poseDataForClient) {
                        delete poseDataForClient.ground_location
                    }
                    if ('action_detection' in poseDataForClient) {
                        delete poseDataForClient.action_detection
                    }
                    if ('gaze_tracking' in poseDataForClient) {
                        delete poseDataForClient.gaze_tracking
                    }
                    if ('fitting' in poseDataForClient) {
                        delete poseDataForClient.fitting
                    }
                    if (clientSubscriptionMap.has(ws)) {
                        const featureSubscriptions = clientSubscriptionMap.get(ws)
                        if (featureSubscriptions.indexOf('ground_location') >= 0) {
                            poseDataForClient.ground_location = pose.ground_location
                        }
                        if (featureSubscriptions.indexOf('action_detection') >= 0) {
                            poseDataForClient.action_detection = pose.action_detection
                        }
                        if (featureSubscriptions.indexOf('gaze_tracking') >= 0) {
                            poseDataForClient.gaze_tracking = pose.gaze_tracking
                        }
                        if (featureSubscriptions.indexOf('fitting') >= 0) {
                            poseDataForClient.fitting = pose.fitting
                        }
                    }
                    if (ws.useJson) {
                        messageContent = JSON.stringify(poseDataForClient)
                        ws.send(messageContent)
                    } else if (pose.flatbuffersData) {
                        ws.send(Buffer.from(pose.flatbuffersData))
                    }
                }
            });
        } else if(type === MessageType.ApplicationControl) {
            const controlData = message.control()
            if (controlData.action() === 'subscribe') {
                if (!clientSubscriptionMap.has(ws)) {
                    clientSubscriptionMap.set(ws, [])
                }
                clientSubscriptionMap.get(ws).push(controlData.featureId())
                console.log(`client with id "${clientIdMap.get(ws)}" subscribe ${controlData.featureId()}`)
            } else if (controlData.action() === 'release') {
                if (clientSubscriptionMap.has(ws)) {
                    const clientSubscription = clientSubscriptionMap.get(ws)
                    const subscriptionIndex = clientSubscription.indexOf(controlData.featureId())
                    if (subscriptionIndex >= 0) {
                        clientSubscription.splice(subscriptionIndex, 1)
                        console.log(`client with id "${clientIdMap.get(ws)}" release ${controlData.featureId()}`)
                    }
                }
            } else if (controlData.featureId() === 'imu' && controlData.action() === 'config') {
                // console.log(message)
                messageBuffer.imuFPS = controlData.data().fps()
            }
        } else if (type === MessageType.ApplicationClient) {
            activeApplicationClient.push(ws)
            ws.notActived = false
            const id = message.client().id()
            if (id) {
                clientMap.set(id, ws)
                clientIdMap.set(ws, id)
                ws.useJson = message.client().useJson()
            }
            console.log(`application client with id "${id}" attached`)
        } else if (type === MessageType.SensorClient) {
            messageBuffer.createSensorBuffer(message.sensorClient().sensorType())
        } else if (bufferData.jsonMessage.type === 'sensor_frame' || bufferData.jsonMessage.type === 'sensor_control') {
            bufferData.jsonMessage.type = 'application_frame'
            activeApplicationClient.forEach(function(ws){
                if(!ws.notActived) {
                    messageContent = JSON.stringify(bufferData.jsonMessage)
                    ws.send(messageContent)
                }
            })
        }
    }
});

console.log("server started")
wss = new WebSocketServer({ port: 8181 });
wss.on('connection', function (ws) {
    console.log('client connected');
    ws.on('message', function (message) {
        //do fps calculator 
        const now = performance.now();
        while (times.length > 0 && times[0] <= now - 1000) {
            times.shift();
        }
        times.push(now);
        fps = times.length;

        if(counter > 100) {
            counter = 0
            console.log('fps from sensor: ' + fps)
        }
        counter = counter + 1;
        var inputData
        var jsonMessage
        // console.log('input data:'+message)
        try {
            // 兼容JSON格式的数据
            messageContent = message.toString('ascii');
            jsonMessage = JSON.parse(messageContent);
            inputData = jsonMessageToFlatbuffers(jsonMessage)
        } catch (error) {
            console.log('error: '+error)
            // 传入数据为flatbuffers字节流
            inputData = message
            // console.log('input flatbuffers:'+inputData)
        }
        // console.log(JSON.stringify(message)+" flatbuffers:"+inputData)
        var inputMessage = InputMessage.getRootAsInputMessage(new flatbuffers.ByteBuffer(inputData))
        messageBuffer.addNewMessage(inputMessage, ws, inputData, jsonMessage)
        setImmediate (function(){
            const theMessage = messageBuffer.peekMessage()
            if (theMessage !== undefined) {
                messageLoop(theMessage)

            }
        })
    });
    ws.on('close', function (event) {
        ws.notActived = true
        console.log('disconnected');
    });
    ws.on('error', function (error) {
        ws.notActived = true
        console.log('error occured');
    });
}); 
function coroutine(f) {
    var o = f(); // instantiate the coroutine
    o.next(); // execute until the first yield
    return function(x) {
        o.next(x);
    }
}

function jsonMessageToFlatbuffers(message) {
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
            if(message.data.height) ControlData.addHeight(builder, message.data.height)
            if(message.data.fps) ControlData.addFps(builder, message.data.fps)
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

//启动串口连接dongle程序
var workerProcess = childProcess.exec('node ../client/imuClient.js ', function (error, stdout, stderr) {
    if (error) {
        console.log(error.stack);
        console.log('Error code: '+error.code);
        console.log('Signal received: '+error.signal);
    }
    console.log('stdout: ' + stdout);
    console.log('stderr: ' + stderr);
});

workerProcess.on('exit', function (code) {
    console.log('子进程已退出，退出码 '+code);
});
