var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
var midware = require('./src/midware');

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
var times = [];
counter = 0

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
        type = message.type
        if(type === 'pose_landmark') {
            //depth correction
            
            //TODO action midware here
            message.action = ["",""]
            
            activeApplicationClient.forEach(function(ws){
                if(ws.notActived === false) {
                    ws.send( messageContent)
                }
            });
        }
        if(type === 'application_client') {
            activeApplicationClient.push(ws)
            ws.notActived = false
            console.log("application client attached")
        }
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