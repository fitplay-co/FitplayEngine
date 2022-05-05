var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
const groundLocation = require('./src/midware_ground_location');
const actionDetection = require('./src/midware_action_detection');
const gazeTracking = require('./src/midware_gaze_tracking');
//const { type } = require('os');
const {performance} = require('perf_hooks')

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

//start server app 
var WebSocketServer = require('ws').Server
var activeApplicationClient = []
var clientMap = new Map() // id -> client映射
var clientIdMap = new Map() // client -> id映射
var clientSubscriptionMap = new Map()
var times = [];
var counter = 0
var processingJob = 0

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
        processingJob = processingJob +1 
        setImmediate (function(){
            message = JSON.parse(messageContent);
            type = message.type
            if(type === 'pose_landmark') {
                //TODO for now only pose provided in message as pose landmark
                pose = message
                //jump if process jobs too much 
                if(processingJob < 4) {
                    //TODO process Input here for input 
                    //此处开始写局部坐标的初始化（地面坐标系）
                    //depth correction
                    //readPose.process(pose)
                    groundLocation.process(pose)
                    actionDetection.process(pose)
                    gazeTracking.process(pose)
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
                    messageContent = JSON.stringify(pose)
                    // console.log(pose.timeProfiling)
                    activeApplicationClient.forEach(function(ws){
                        if(ws.notActived === false) {
                            ws.send( messageContent)
                        }
                    });
                    if (clientSubscriptionMap.get('pose_landmark')) {
                        clientSubscriptionMap.get('pose_landmark').forEach(ws => {
                            if(!ws.notActived) {
                                ws.send(messageContent)
                            }
                        })
                    }
                } else {
                    console.log("warning: frame jump ")
                }
            } else if(type ==='application_control'){
                pose = message
                //console.log(pose)
                if(processingJob < 4){
                    if(pose.feature_id === 'ground_location'){
                        groundLocation.process(pose)
                    }
                    if (message.action === 'subsribe') {
                        if (!clientSubscriptionMap.has(message.feature_id)) {
                            clientSubscriptionMap.set(message.feature_id, [])
                        }
                        clientSubscriptionMap.get(message.feature_id).push(ws)
                        console.log(`client with id "${clientIdMap.get(ws)}" subscribe ${message.feature_id}`)
                    } else if (message.action === 'release') {
                        if (clientSubscriptionMap.has(message.feature_id)) {
                            const clients = clientSubscriptionMap.get(message.feature_id)
                            if (clients.indexOf(ws) >= 0) {
                                clients.splice(clients.indexOf(ws), 1)
                                console.log(`client with id "${clientIdMap.get(ws)}" release ${message.feature_id}`)
                            }
                        }
                    }
                }
                
            } else if(type === 'application_client') {
                activeApplicationClient.push(ws)
                ws.notActived = false
                if (message.id) {
                    clientMap.set(message.id, ws)
                    clientIdMap.set(ws, message.id)
                }
                console.log(`application client with id "${message.id}" attached`)
            }
            processingJob = processingJob - 1 
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