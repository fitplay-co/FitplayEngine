// client
var WebSocketClient = require('websocket').client;
var client = new WebSocketClient();
// frame command
const readline = require('readline');
const r1 = readline.createInterface({
    input:process.stdin,
    output:process.stdout
});
// read csv
var fs = require('fs')
var path = require('path')
var file = path.join(__dirname, '../modules/test/data/test_data_heavy.csv')
var textByLine = fs.readFileSync(file).toString().split("\n")
const res = textByLine.map((line) => line.split(","))
var arr = []
for (var i=0;i<res.length;i++)
{ 
    if(res[i][0] == 'Frame') {
        arr.push(i)
    }
}
var index = 0
var len = arr.length;
//client message
var sensorClientMessage = {
    "type":"sensor_client",
    "sensor_id":"rgbd_replay",
    "sensor_type":"pose_landmark",
    "sensor_info":{
        "video_width":"",
        "video_height":""
    }
}
var sensorFrame = {
    "type":"sensor_frame",
    "sensor_id":"rgbd_replay",
    "sensor_type":"pose_landmark",
    "pose_landmark":{
        "keypoints":[],
        "keypoints3D":[],
        "timestamp":1,
        "version":"0.1.0"
    }
}
function genFrameData()
{
    sensorFrame.pose_landmark.timestamp = index
    for(var i = 0; i<33; i++) {
        sensorFrame.pose_landmark.keypoints.push({
            "name" : res[arr[index] + 2 + i][0],
            "x" : res[arr[index] + 2 + i][1],
            "y" : res[arr[index] + 2 + i][2],
            "z" : res[arr[index] + 2 + i][3]
        })
    }
    
    for(var i = 0; i<33; i++) {
        sensorFrame.pose_landmark.keypoints3D.push({
            "name" : res[arr[index] + 36 + i][0],
            "x" : res[arr[index] + 36 + i][1],
            "y" : res[arr[index] + 36 + i][2],
            "z" : res[arr[index] + 36 + i][3]
        })
    }
}

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
        }
    });
    connection.send(JSON.stringify(sensorClientMessage))
    //set command reference
    r1.setPrompt('cmd:');
    //prompt waiting for input
    r1.prompt();
    r1.on('line',function (line){
        switch(line.trim()){
            case 'a':
                index = index - 1
                if(index < 0) break;
                genFrameData()
                console.log(JSON.stringify(sensorFrame))
                connection.send(JSON.stringify(sensorFrame))
                console.log('backward frame');
                break;
            case 'd':
                index = index + 1
                if(index == len - 1) break;
                genFrameData()
                console.log(JSON.stringify(sensorFrame))
                connection.send(JSON.stringify(sensorFrame))
                console.log('forward frame');
                break;
            case 'f':
                for(var j = 0; j < 100; j++) {
                    index = index + 1
                    if(index == len - 1) break;
                    genFrameData()
                    console.log(JSON.stringify(sensorFrame))
                    connection.send(JSON.stringify(sensorFrame))
                }
                break;
            case 'close':
                r1.close();
                break;
            default:
                console.log('undefined command');
                break;
        }
        // listening for line event
        r1.prompt();
    });
    r1.on('close',function (){
        console.log('----end----');
        process.exit(0);
    });
    // setTimeout(() => {
    //     connection.send(JSON.stringify(actionDetectionRelease))
    // }, 10000);
});
client.connect('ws://localhost:8181/');