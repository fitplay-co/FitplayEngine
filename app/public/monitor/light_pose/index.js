let ws=null;
let url='ws://localhost:8181/';
let isReconnect=false;
websocketConnect();

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
    addData(e.data);
  }

  var appClientMessage = {
    "type" : "application_client",
    "id": "test_client"
  }

  setTimeout(() => {
    ws_send(JSON.stringify(appClientMessage))
}, 1000);
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
// var arr = new Array(100).fill(0);
var arr = []
for(let i = 1,len=100;i<=len;i++){arr.push(i)}
var count = 0

var myChart1 = echarts.init(document.getElementById('chart1'));
var myChart2 = echarts.init(document.getElementById('chart2'));
var myChart3 = echarts.init(document.getElementById('chart3'));
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
        text: 'Monitor',
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
      text: 'Monitor',
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
    symbol: "none",
    data: num2,
    type: "line"
  }]
});

function addData(inputData){
    var select1 = document.getElementById('keypoints').options[document.getElementById('keypoints').selectedIndex];
    var select_type1 = document.getElementById('keypoints_type').options[document.getElementById('keypoints_type').selectedIndex];
    var select_data1 = document.getElementById('keypoints_data').options[document.getElementById('keypoints_data').selectedIndex];
    var value_type1 = select_type1.value;
    var value_data1 = select_data1.value;
    var value1 = select1.value;
    var text1 = select1.text;
    let jsonObj=JSON.parse(inputData);
    // console.log(jsonObj)
    var m1 = 0;
    if (value_data1 == 1 && value_type1 == 1) m1 = jsonObj.pose_landmark.keypoints[value1].x
    if (value_data1 == 2 && value_type1 == 1) m1 = jsonObj.pose_landmark.keypoints[value1].y
    if (value_data1 == 3 && value_type1 == 1) m1 = jsonObj.pose_landmark.keypoints[value1].z

    if (value_data1 == 1 && value_type1 == 2) m1 = jsonObj.pose_landmark.keypoints3D[value1].x
    if (value_data1 == 2 && value_type1 == 2) m1 = jsonObj.pose_landmark.keypoints3D[value1].y
    if (value_data1 == 3 && value_type1 == 2) m1 = jsonObj.pose_landmark.keypoints3D[value1].z
    console.log("m1 = " + m1)
    num1.push(m1)
    num1.shift()
    num2.push(jsonObj.monitor.m1)
    console.log("m2 = " + jsonObj.monitor.m1)
    num2.shift()
    num3.push(jsonObj.monitor.m2)
    console.log("m3 = " + jsonObj.monitor.m2)
    num3.shift()
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
          subtext: jsonObj.monitor.m1Name
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
        subtext: jsonObj.monitor.m2Name
      },
      yAxis: {
        type: "value"
      },
      series: [{
        data: num3
      }]
  });
}