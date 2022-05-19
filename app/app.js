var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
const groundLocation = require('./src/midware_ground_location');
const actionDetection = require('./src/midware_action_detection');
const wasm = require('./src/midware_wasm');
const gazeTracking = require('./src/midware_gaze_tracking');
//const { type } = require('os');
const {performance} = require('perf_hooks');
const messageBuffer = require('./src/message_buffer');

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

const ADVANCED_FEATURES = ['ground_loccation', 'action_detection', 'gaze_tracking', 'throw_direction']

//start server app 
var WebSocketServer = require('ws').Server
var activeApplicationClient = []
var clientMap = new Map() // id -> client映射
var clientIdMap = new Map() // client -> id映射
var clientSubscriptionMap = new Map() // client订阅的高级能力
var advancedFeaturesSubscriptionsMap = new Map() // 每个高级能力订阅的客户端数
var times = [];
var counter = 0
var processingJob = 0

// 创建消息处理循环
var messageLoop = coroutine(function*() {
    var bufferData;
    while (bufferData = yield) { // 等待下一个消息
        const message = bufferData.message
        const ws = bufferData.webSocket
        type = message.type
        messageTime = Date.now()
        if(type === 'pose_landmark') {
            //TODO for now only pose provided in message as pose landmark
            pose = message
            pose.timeProfiling.serverReceive = messageTime
            pose.timeProfiling.processingTime = Date.now()
            //jump if process jobs too much 
            //TODO process Input here for input 
            //此处开始写局部坐标的初始化（地面坐标系）
            //depth correction
            //readPose.process(pose)

            // 只有当至少1个客户端订阅了高级能力时再进行相关计算
            wasm.process(pose)
            var groundLocationData, actionDetectionData, gazeTrackingData
            if (advancedFeaturesSubscriptionsMap.has('ground_loccation')) {
                groundLocationData = groundLocation.process(pose)
            }
            if (advancedFeaturesSubscriptionsMap.has('action_detection')) {
                actionDetectionData = actionDetection.process(pose)
            }
            if (advancedFeaturesSubscriptionsMap.has('gaze_tracking')) {
                gazeTrackingData = gazeTracking.process(pose)
            }
            //调整pose结构适配api格式
            pose.type = "application_frame"
            pose.pose_landmark = {
                keypoints: pose.keypoints,
                keypoints3D: pose.keypoints3D,
                timestamp: Date.now(),
                version:"0.1.0"
            }
            // console.log(pose["fitting"])
            delete pose.keypoints
            delete pose.keypoints3D
            pose.timeProfiling.beforeSendTime = Date.now()
            // console.log(pose.timeProfiling)
            activeApplicationClient.forEach(function(ws){
                if(!ws.notActived) {
                    if (clientSubscriptionMap.has(ws)) {
                        const featureSubscriptions = clientSubscriptionMap.get(ws)
                        if (featureSubscriptions.indexOf('ground_loccation') >= 0) {
                            pose.ground_location = groundLocationData.ground_location
                        }
                        if (featureSubscriptions.indexOf('action_detection') >= 0) {
                            pose.action_detection = actionDetectionData.action_detection
                        }
                        if (featureSubscriptions.indexOf('gaze_tracking') >= 0) {
                            pose.gaze_tracking = gazeTrackingData.gaze_tracking
                        }
                    }
                    messageContent = JSON.stringify(pose)
                    ws.send(messageContent)
                }
            });
        } else if(type ==='application_control') {
            if (message.action === 'subsribe') {
                if (!clientSubscriptionMap.has(ws)) {
                    clientSubscriptionMap.set(ws, [])
                }
                clientSubscriptionMap.get(ws).push(message.feature_id)
                if (advancedFeaturesSubscriptionsMap.has(message.feature_id)) {
                    advancedFeaturesSubscriptionsMap.set(message.feature_id, advancedFeaturesSubscriptionsMap.get(message.feature_id) + 1)
                } else {
                    advancedFeaturesSubscriptionsMap.set(message.feature_id, 1)
                }
                console.log(`client with id "${clientIdMap.get(ws)}" subscribe ${message.feature_id}`)
            } else if (message.action === 'release') {
                if (clientSubscriptionMap.has(ws)) {
                    const clientSubscription = clientSubscriptionMap.get(ws)
                    const subscriptionIndex = clientSubscription.indexOf(message.feature_id)
                    if (subscriptionIndex >= 0) {
                        clientSubscription.splice(subscriptionIndex, 1)
                        console.log(`client with id "${clientIdMap.get(ws)}" release ${message.feature_id}`)
                    }
                }
            }
        } else if (type === 'application_client') {
            activeApplicationClient.push(ws)
            ws.notActived = false
            if (message.id) {
                clientMap.set(message.id, ws)
                clientIdMap.set(ws, message.id)
            }
            console.log(`application client with id "${message.id}" attached`)
        } else if (type === 'sensor_client') {
            messageBuffer.createSensorBuffer(message.sensor_type)
        } else if (type === 'sensor_frame' && message.sensor_type === 'imu') {
            activeApplicationClient.forEach(function(ws){
                if(!ws.notActived) {
                    messageContent = JSON.stringify(message)
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

        messageContent = message.toString('ascii');
        message = JSON.parse(messageContent);
        messageBuffer.addNewMessage(message, ws)
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