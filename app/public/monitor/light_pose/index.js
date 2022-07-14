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
// var arr = new Array(100).fill(0);
var arr = []
for(let i = 1,len=100;i<=len;i++){arr.push(i)}
var count = 0
var myChart1 = echarts.init(document.getElementById('chart1'));
var myChart2 = echarts.init(document.getElementById('chart2'));
myChart1.setOption({
    title: {
        text: 'Monitor',
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
function addData(inputData){
    let jsonObj=JSON.parse(inputData);
    console.log(jsonObj.monitor)
    num1.push(jsonObj.monitor.m1)
    num1.shift()
    num2.push(jsonObj.monitor.m2)
    num2.shift()
    myChart1.setOption({
        title: {
          subtext: jsonObj.monitor.m1Name
        },
        series: [{
          data: num1
        }]
    });
    myChart2.setOption({
        title: {
          subtext: jsonObj.monitor.m2Name
        },
        series: [{
          data: num2
        }]
    });
}