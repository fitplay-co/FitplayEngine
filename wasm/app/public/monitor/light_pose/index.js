let ws=null;
let url='ws://localhost:8181/';
let isReconnect=false;
websocketConnect();

let dataType = 0;

function websocketConnect(){ 
  try {
    ws = new WebSocket(url);
    ws_init();

  } catch (error) {
    console.error("websocket_connect error!");
    websocketReconnect();
  }
}

function websocketReconnect(){
  if(isReconnect==true)
  {
    return;
  }
  isReconnect=true;
  console.log("will reconnect "+url);
  setTimeout(()=>{
    websocketConnect();
    isReconnect=false;
  },2000);
}

function ws_init(){
  ws.onopen = () => {
    //alert("bbb ");
    //ws.send('Message From Client') ;
  }

  ws.onclose = function () {
    console.log('onclose');
    websocketReconnect();
  };

  ws.onerror = (error) => {
   // alert("WebSocket onerror "+error);
    websocketReconnect();
  }
  
  ws.onmessage = (e) =>{
    //DisplayData(e.data);
    if(dataType == 1) {
      addFlatData(e.data);
    }
    else {
      addData(e.data);
    }
  }

  var appClientMessage = {
    "type" : "application_client",
    "id": "test_client",
    "useJson" : true
  }

  var appClientFlatMessage = {
    "type" : "application_client",
    "id" : "monitor_client"
  }

  var actionDetectionSubscribe = {
    "type" : "application_control",
    "feature_id" : "action_detection",
    "action" : "subscribe" 
  }

  var generalDetectionSubscribe = {
    "type" : "application_control",
    "feature_id" : "general_detection",
    "action" : "subscribe" 
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

  var heightControl = {
    "type" : "application_control",
    "feature_id" : "action_detection",
    "action" : "set_player",
    "data": {
        "height": 178
    }
}
if(dataType == 1) {
  setTimeout(() => {
    ws_send(JSON.stringify(appClientFlatMessage))
}, 1000);
}
else {
  setTimeout(() => {
    ws_send(JSON.stringify(appClientMessage))
}, 1000);
}
  setTimeout(() => {
    ws_send(JSON.stringify(actionDetectionSubscribe))
}, 1000);
  setTimeout(() => {
    ws_send(JSON.stringify(generalDetectionSubscribe))
}, 1000);
  setTimeout(() => {
    ws_send(JSON.stringify(groundLocationSubscribe))
}, 1000);
  setTimeout(() => {
    ws_send(JSON.stringify(gazeSubscribe))
}, 1000);
  setTimeout(() => {
    ws_send(JSON.stringify(fittingSubscribe))
}, 1000);
//   setTimeout(() => {
//     ws_send(JSON.stringify(heightControl))
// }, 1000);
}

function ws_send(data){
  if(ws.readyState==WebSocket.OPEN)
  {
    console.log(data);
    ws.send(data);
  }
}

var num1 = new Array(100).fill(0);
var num2 = new Array(100).fill(0);
var num3 = new Array(100).fill(0);
var num4 = new Array(100).fill(0);
// var arr = new Array(100).fill(0);
var arr = []
for(let i = 1,len=100;i<=len;i++){arr.push(i)}
var count = 0

var myChart1 = echarts.init(document.getElementById('chart1'));
var myChart2 = echarts.init(document.getElementById('chart2'));
var myChart3 = echarts.init(document.getElementById('chart3'));
var myChart4 = echarts.init(document.getElementById('chart4'));
myChart1.setOption({
    title: {
        text: 'Keypoint Monitor',
        subtext: 'M1'
    },
    xAxis: {
      type: "category",
      name: "time",
      data: arr
    },
    yAxis: {
      type: "value"
    },
    series: [{
      symbol: "none",
      data: num1,
      type: "line"
    }]
});
myChart2.setOption({
    title: {
        text: 'Keypoint Monitor',
        subtext: 'M2'
    },
    xAxis: {
      type: "category",
      name: "time",
      data: arr
    },
    yAxis: {
      type: "value"
    },
    series: [{
      symbol: "none",
      data: num2,
      type: "line"
    }]
});
myChart3.setOption({
  title: {
      text: 'Keypoint Monitor',
      subtext: 'M3'
  },
  xAxis: {
    type: "category",
    name: "time",
    data: arr
  },
  yAxis: {
    type: "value"
  },
  series: [{
    // symbol: "none",
    data: num3,
    type: "line"
  }]
});
myChart4.setOption({
  title: {
      text: 'API Monitor',
      subtext: 'M4'
  },
  xAxis: {
    type: "category",
    name: "time",
    data: arr
  },
  yAxis: {
    type: "value"
  },
  series: [{
    // symbol: "none",
    data: num4,
    type: "line"
  }]
});

function addFlatData(inputData){
    // console.log(inputData)
    var buf = new flatbuffers.ByteBuffer(inputData);
    var flatData = OutputMessage.getRootAsOutputMessage(buf);
    var test = flatData.version();
    console.log(test)
}

function addData(inputData){
    var select1 = document.getElementById('keypoints').options[document.getElementById('keypoints').selectedIndex];
    var select_type1 = document.getElementById('keypoints_type').options[document.getElementById('keypoints_type').selectedIndex];
    var select_data1 = document.getElementById('keypoints_data').options[document.getElementById('keypoints_data').selectedIndex];
    var value1 = select1.value;
    var text1 = select1.text;
    var value_type1 = select_type1.value;
    var value_data1 = select_data1.value;

    var select2 = document.getElementById('keypoints_2').options[document.getElementById('keypoints_2').selectedIndex];
    var select_type2 = document.getElementById('keypoints_type_2').options[document.getElementById('keypoints_type_2').selectedIndex];
    var select_data2 = document.getElementById('keypoints_data_2').options[document.getElementById('keypoints_data_2').selectedIndex];
    var value2 = select2.value;
    var text2 = select2.text;
    var value_type2 = select_type2.value;
    var value_data2 = select_data2.value;

    var select3 = document.getElementById('keypoints_3').options[document.getElementById('keypoints_3').selectedIndex];
    var select_type3 = document.getElementById('keypoints_type_3').options[document.getElementById('keypoints_type_3').selectedIndex];
    var select_data3 = document.getElementById('keypoints_data_3').options[document.getElementById('keypoints_data_3').selectedIndex];
    var value3 = select3.value;
    var text3 = select3.text;
    var value_type3 = select_type3.value;
    var value_data3 = select_data3.value;

    var select4 = document.getElementById('Walk_API').options[document.getElementById('Walk_API').selectedIndex];
    var value4 = select4.value;
    var text4 = select4.text;

    let jsonObj=JSON.parse(inputData);
    // console.log(jsonObj)
    var m1 = 0;
    if (value_data1 == 1 && value_type1 == 1) m1 = jsonObj.pose_landmark.keypoints[value1].x
    if (value_data1 == 2 && value_type1 == 1) m1 = jsonObj.pose_landmark.keypoints[value1].y
    if (value_data1 == 3 && value_type1 == 1) m1 = jsonObj.pose_landmark.keypoints[value1].z

    if (value_data1 == 1 && value_type1 == 2) m1 = jsonObj.pose_landmark.keypoints3D[value1].x
    if (value_data1 == 2 && value_type1 == 2) m1 = jsonObj.pose_landmark.keypoints3D[value1].y
    if (value_data1 == 3 && value_type1 == 2) m1 = jsonObj.pose_landmark.keypoints3D[value1].z

    var m2 = 0;
    if (value_data2 == 1 && value_type2 == 1) m2 = jsonObj.pose_landmark.keypoints[value2].x
    if (value_data2 == 2 && value_type2 == 1) m2 = jsonObj.pose_landmark.keypoints[value2].y
    if (value_data2 == 3 && value_type2 == 1) m2 = jsonObj.pose_landmark.keypoints[value2].z

    if (value_data2 == 1 && value_type2 == 2) m2 = jsonObj.pose_landmark.keypoints3D[value2].x
    if (value_data2 == 2 && value_type2 == 2) m2 = jsonObj.pose_landmark.keypoints3D[value2].y
    if (value_data2 == 3 && value_type2 == 2) m2 = jsonObj.pose_landmark.keypoints3D[value2].z

    var m3 = 0;
    if (value_data3 == 1 && value_type3 == 1) m3 = jsonObj.pose_landmark.keypoints[value3].x
    if (value_data3 == 2 && value_type3 == 1) m3 = jsonObj.pose_landmark.keypoints[value3].y
    if (value_data3 == 3 && value_type3 == 1) m3 = jsonObj.pose_landmark.keypoints[value3].z

    if (value_data3 == 1 && value_type3 == 2) m3 = jsonObj.pose_landmark.keypoints3D[value3].x
    if (value_data3 == 2 && value_type3 == 2) m3 = jsonObj.pose_landmark.keypoints3D[value3].y
    if (value_data3 == 3 && value_type3 == 2) m3 = jsonObj.pose_landmark.keypoints3D[value3].z

    var m4 = 0;
    switch(text4) {
      case "leftLeg" :
        m4 = jsonObj.action_detection.walk.leftLeg;
        break;
      case "rightLeg" :
        m4 = jsonObj.action_detection.walk.rightLeg;
        break;
      case "leftFrequency" :
        m4 = jsonObj.action_detection.walk.leftFrequency;
        break;
      case "rightFrequency" :
        m4 = jsonObj.action_detection.walk.rightFrequency;
        break;
      case "leftHipAng" :
        m4 = jsonObj.action_detection.walk.leftHipAng;
        break;
      case "rightHipAng" :
        m4 = jsonObj.action_detection.walk.rightHipAng;
        break; 
      case "leftStepLength" :
        m4 = jsonObj.action_detection.walk.leftStepLength;
        break; 
      case "rightStepLength" :
        m4 = jsonObj.action_detection.walk.rightStepLength;
        break; 
      case "leftProgress" :
        m4 = jsonObj.action_detection.walk.leftProgress;
        break; 
      case "rightProgress" :
        m4 = jsonObj.action_detection.walk.rightProgress;
        break; 
      case "turn" :
        m4 = jsonObj.action_detection.walk.turn;
        break; 
      case "stepRate" :
        m4 = jsonObj.action_detection.walk.stepRate;
        break; 
      case "stepLen" :
        m4 = jsonObj.action_detection.walk.stepLen;
        break; 
      case "velocity" :
        m4 = jsonObj.action_detection.walk.velocity;
        break; 
      case "velocityThreshold" :
        m4 = jsonObj.action_detection.walk.velocityThreshold;
        break;
      case "realtimeLeftLeg" :
        m4 = jsonObj.action_detection.walk.realtimeLeftLeg;
        break; 
      case "realtimeRightLeg" :
        m4 = jsonObj.action_detection.walk.realtimeRightLeg;
        break; 
    }

    num1.push(m1)
    num1.shift()
    // console.log("m1 = " + m1)
    // console.log(jsonObj)
    num2.push(m2)
    num2.shift()
    // num2.push(jsonObj.monitor.m1)
    // num2.shift()
    // console.log("m2 = " + jsonObj.monitor.m1)
    num3.push(m3)
    num3.shift()
    // num3.push(jsonObj.monitor.m2)
    // num3.shift()
    // console.log("m3 = " + jsonObj.monitor.m2)
    num4.push(m4)
    num4.shift()
    // console.log("m4 = " + m4)

    myChart1.setOption({
        title: {
          subtext: text1
        },
        yAxis: {
          type: "value"
        },
        series: [{
          data: num1
        }]
    });
    myChart2.setOption({
        title: {
          subtext: text2
        },
        yAxis: {
          type: "value"
        },
        series: [{
          data: num2
        }]
    });
    myChart3.setOption({
      title: {
        subtext: text3
      },
      yAxis: {
        type: "value"
      },
      series: [{
        data: num3
      }]
    });
    myChart4.setOption({
      title: {
        subtext: text4
      },
      yAxis: {
        type: "value"
      },
      series: [{
        data: num4
      }]
    });
}