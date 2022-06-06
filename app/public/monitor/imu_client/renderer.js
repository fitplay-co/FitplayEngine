const device_id=document.getElementById("device_id");
const timestamp1=document.getElementById("timestamp1");
const seq=document.getElementById("seq");
const acc_x=document.getElementById("acc_x");
const acc_y=document.getElementById("acc_y");
const acc_z=document.getElementById("acc_z");
const gyr_x=document.getElementById("gyr_x");
const gyr_y=document.getElementById("gyr_y");
const gyr_z=document.getElementById("gyr_z");
const mag_x=document.getElementById("mag_x");
const mag_y=document.getElementById("mag_y");
const mag_z=document.getElementById("mag_z");

const timestamp2=document.getElementById("timestamp2");
const key1=document.getElementById("key1");
const key2=document.getElementById("key2");
const linear_key1=document.getElementById("linear_key1");
const linear_key2=document.getElementById("linear_key2");
const joystick_x=document.getElementById("joystick_x");
const joystick_y=document.getElementById("joystick_y");
const joystick_key=document.getElementById("joystick_key");
const heart_rate=document.getElementById("heart_rate");
const blood_oxygen=document.getElementById("blood_oxygen");

//device2
const device_id2=document.getElementById("device_id2");
const timestamp12=document.getElementById("timestamp12");
const seq2=document.getElementById("seq2");
const acc_x2=document.getElementById("acc_x2");
const acc_y2=document.getElementById("acc_y2");
const acc_z2=document.getElementById("acc_z2");
const gyr_x2=document.getElementById("gyr_x2");
const gyr_y2=document.getElementById("gyr_y2");
const gyr_z2=document.getElementById("gyr_z2");
const mag_x2=document.getElementById("mag_x2");
const mag_y2=document.getElementById("mag_y2");
const mag_z2=document.getElementById("mag_z2");
const heart_rate2=document.getElementById("heart_rate2");
const blood_oxygen2=document.getElementById("blood_oxygen2");
const quaternions=document.getElementById("quaternions");
const quaternions2=document.getElementById("quaternions2");

const timestamp22=document.getElementById("timestamp22");
const key12=document.getElementById("key12");
const key22=document.getElementById("key22");
const key_menu=document.getElementById("key_menu");
const key_menu2=document.getElementById("key_menu2");

const linear_key12=document.getElementById("linear_key12");
const linear_key22=document.getElementById("linear_key22");
const joystick_x2=document.getElementById("joystick_x2");
const joystick_y2=document.getElementById("joystick_y2");
const joystick_key2=document.getElementById("joystick_key2");


const btn = document.getElementById('btn');
const btn2 = document.getElementById('btn2');
const vibration_type = document.getElementById('vibration_type');
const vibration_type2 = document.getElementById('vibration_type2');
const strength = document.getElementById('strength');
const strength2 = document.getElementById('strength2');
let deviceId="";
let deviceId2="";


// 打开一个 web socket
let ws=null;
//let url='ws://10.0.88.113:9081';
let url='ws://localhost:8181/';
let isRconnect=false;


websocketConnect();

function websocketConnect(){ 
  try {
  // 打开一个 web socket
   ws = new WebSocket(url);

  ws_init();

  } catch (error) {
    console.error("websocket_connect error!");
    websocketReconnect();
  }
}

function websocketReconnect(){
  if(isRconnect==true)
  {
    return;
  }

  isRconnect=true;
  console.log("will reconnect "+url);
  setTimeout(()=>{
    websocketConnect();
    isRconnect=false;
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

  var appClientMessage =  {
    "type" : "application_client",
    "id": "imu_client"
  }

  setTimeout(() => {
    ws_send(JSON.stringify(appClientMessage))
}, 1000);
  // ws.send(JSON.stringify(appClientMessage));
}


function ws_send(data){
  if(ws.readyState==WebSocket.OPEN)
  {
    console.log(data);
    ws.send(data);
  }
}

let imuData=[{"index":0,"jsonData":null},{"index":0,"jsonData":null},{"index":0,"jsonData":null},{"index":0,"jsonData":null}];
const MaxCout=100;

function addData(inputData){
  let jsonObj=JSON.parse(inputData);
  let index=0;
  let deviceId=jsonObj.device_id;
  let type=jsonObj.sensor_type;
  let imuJsonData;

  //console.log(jsonObj.type);

  if(type!="imu" && type!="input")
  {
     //console.log("data is not monitor need! type:%s ",type);
     return;
  }

  switch (type) {
    case "imu":
      if(deviceId==0)
      {
        index=0;
      }
      else
      {
        index=1;
      }
      break;

    case "input":
      if(deviceId==0)
      {
        index=2;
      }
      else
      {
        index=3;
      }
    break;

    default:
      break;
  }


  if(imuData[index].index==0)
  {
    imuData[index].index++;
    imuData[index].jsonData=inputData;
    return ;
  }
  else
  {
    imuJsonData=JSON.parse(imuData[index].jsonData);
 
    for(var val in imuJsonData)
    {
      //console.log(val);
      if(val=="timestamp"){
      imuJsonData[val]+=jsonObj[val];
      }
      
      //目前只平均计算了accelerometer.x的值
      if(val=="accelerometer" && (index==0 || index==1)){
        imuJsonData[val].x = Number(imuJsonData[val].x)+Number(jsonObj[val].x);
       // imuJsonData[val].accelerometer.y+=jsonObj[val].accelerometer.y;
       // imuJsonData[val].accelerometer.z+=jsonObj[val].accelerometer.z;
      } 
    }

    imuData[index].jsonData=JSON.stringify(imuJsonData);
  }
  
  imuData[index].index++;

  if(imuData[index].index>=MaxCout)
  {
    tempJsonData=JSON.parse(imuData[index].jsonData);
    imuData[index].index=0;
    tempJsonData.timestamp=(tempJsonData.timestamp/MaxCout).toFixed(0);
  
    if(index==0 || index==1)
    {
      tempJsonData.accelerometer.x=(tempJsonData.accelerometer.x/MaxCout).toFixed(7);
    }

    imuData[index].jsonData=JSON.stringify(tempJsonData);
    DisplayData(imuData[index].jsonData);
  }
}

btn.addEventListener('click', async () => {
  let jsObj={
            type: "sensor_control",
            sensor_type: "vibration",
            device_id:0,
            vibration_type:0,
            strength:0};

  if(strength.value>100)
  {
    strength.value=100;
  }

  jsObj.device_id=deviceId;
  jsObj.vibration_type=vibration_type.value;
  jsObj.strength=strength.value;
  ws_send(JSON.stringify(jsObj));
 // alert("WebSocket onerror "+JSON.stringify(jsObj));

});

btn2.addEventListener('click', async () => {
  let jsObj={
    type: "sensor_control",
    sensor_type: "vibration",
    device_id:0,
    vibration_type:0,
    strength:0};

    if(strength2.value>100)
    {
      strength2.value=100;
    }

    jsObj.device_id=deviceId2;
    jsObj.vibration_type=vibration_type2.value;
    jsObj.strength=strength2.value;
    ws_send(JSON.stringify(jsObj));
});


let elapsedTime=0,oldTime=0,elapsedTime2=0,oldTime2=0;
const elapsedtime = document.getElementById('elapsedtime');
const elapsedtime2 = document.getElementById('elapsedtime2');

function DisplayData(data) {
  let jsonObj=JSON.parse(data);

  if(deviceId=="" && jsonObj.device_id==0)
  {
    let deviceValue="";

    deviceId=jsonObj.device_id;
    deviceValue=deviceId?"R":"L"; 
    device_id.value=deviceValue+" ["+deviceId+"]";
  }

  if(deviceId2 =="" && jsonObj.device_id==1)
  {
    if(deviceId2=="")
    {
      let device2Value="";
      deviceId2=jsonObj.device_id;
      device2Value=deviceId2?"R":"L";
      device_id2.value=device2Value+" ["+deviceId2+"]";
    }
  }

  if(jsonObj.sensor_type=="imu")
  {

    if(deviceId==jsonObj.device_id)
    {
      timestamp1.value=jsonObj.timestamp;
      seq.value=jsonObj.seq;

      elapsedTime=jsonObj.timestamp-oldTime;
      oldTime=jsonObj.timestamp;
      elapsedtime.value=elapsedTime/MaxCout;
    

      acc_x.value=jsonObj.accelerometer.x;
      acc_y.value=jsonObj.accelerometer.y;
      acc_z.value=jsonObj.accelerometer.z;
      gyr_x.value=jsonObj.gyroscope.x;
      gyr_y.value=jsonObj.gyroscope.y;
      gyr_z.value=jsonObj.gyroscope.z;
      mag_x.value=jsonObj.magnetometer.x;
      mag_y.value=jsonObj.magnetometer.y;
      mag_z.value=jsonObj.magnetometer.z;
      quaternions.value=jsonObj.quaternions.x +","+jsonObj.quaternions.y +","+jsonObj.quaternions.z +","+jsonObj.quaternions.w; 
    }
    else if(deviceId2==jsonObj.device_id)
    {
      timestamp12.value=jsonObj.timestamp;
      seq2.value=jsonObj.seq;
    
      elapsedTime2=jsonObj.timestamp-oldTime2;
      oldTime2=jsonObj.timestamp;
      elapsedtime2.value=elapsedTime2/MaxCout;

      acc_x2.value=jsonObj.accelerometer.x;
      acc_y2.value=jsonObj.accelerometer.y;
      acc_z2.value=jsonObj.accelerometer.z;
      gyr_x2.value=jsonObj.gyroscope.x;
      gyr_y2.value=jsonObj.gyroscope.y;
      gyr_z2.value=jsonObj.gyroscope.z;
      mag_x2.value=jsonObj.magnetometer.x;
      mag_y2.value=jsonObj.magnetometer.y;
      mag_z2.value=jsonObj.magnetometer.z;
      quaternions2.value=jsonObj.quaternions.x +","+jsonObj.quaternions.y +","+jsonObj.quaternions.z +","+jsonObj.quaternions.w; 
    }
  }

  if(jsonObj.sensor_type=="input")
  {
    if(deviceId==jsonObj.device_id)
    {
      timestamp2.value=jsonObj.timestamp;
    
      key1.value=jsonObj.keys.key_A?"弹起":"按下";
      key2.value=jsonObj.keys.key_B?"弹起":"按下";
      key_menu.value=jsonObj.keys.key_menu?"弹起":"按下";

      linear_key1.value=jsonObj.linear_key.L1;
      linear_key2.value=jsonObj.linear_key.L2;

      joystick_x.value=jsonObj.joystick.x;
      joystick_y.value=jsonObj.joystick.y;
      joystick_key.value=jsonObj.joystick.key?"弹起":"按下";
      heart_rate.value=jsonObj.heart_rate;
      blood_oxygen.value=jsonObj.blood_oxygen;

    }
    else if(deviceId2==jsonObj.device_id)
    {
      timestamp22.value=jsonObj.timestamp;
    
      key12.value=jsonObj.keys.key_A?"弹起":"按下";
      key22.value=jsonObj.keys.key_B?"弹起":"按下";
      key_menu2.value=jsonObj.keys.key_menu?"弹起":"按下";

      linear_key12.value=jsonObj.linear_key.L1;
      linear_key22.value=jsonObj.linear_key.L2;

      joystick_x2.value=jsonObj.joystick.x;
      joystick_y2.value=jsonObj.joystick.y;
      joystick_key2.value=jsonObj.joystick.key?"弹起":"按下";
      heart_rate2.value=jsonObj.heart_rate;
      blood_oxygen2.value=jsonObj.blood_oxygen;
    }
  }
}

const display1=document.getElementById("display1");
const display2=document.getElementById("display2");
const DisplayBtn = document.getElementById('DisplayBtn');
let Display1Value;
let Display2Value;
let chart,chart2;

DisplayBtn.addEventListener('click',async () => {
  chart.setTitle({text:'L [0] '+display1.options[display1.selectedIndex].value});
  chart2.setTitle({text:'R [1] '+display2.options[display2.selectedIndex].value});
});

function getChartY(chartIndex){
  let displayValue,y;
  let Display1Value=display1.options[display1.selectedIndex].value;
  let Display2Value=display2.options[display2.selectedIndex].value;

  displayValue=chartIndex?Display2Value:Display1Value;
  
  switch (displayValue) {
      case 'elapsed(ms)':
        y=Number(chartIndex? elapsedtime2.value:elapsedtime.value);
      break;

      case 'acc_x(g)':
        y=Number(chartIndex? acc_x2.value:acc_x.value);
      break;

      case 'acc_y(g)':
        y=Number(chartIndex? acc_y2.value:acc_y.value);
      break;
      case 'acc_z(g)':
        y=Number(chartIndex? acc_z2.value:acc_z.value);
      break;

      case 'gyr_x(dps)':
        y=Number(chartIndex? gyr_x2.value:gyr_x.value);
      break;

      case 'gyr_y(dps)':
        y=Number(chartIndex? gyr_y2.value:gyr_y.value);
      break;
      case 'gyr_z(dps)':
        y=Number(chartIndex? gyr_z2.value:gyr_z.value);
      break;
      case 'heart_rate':
        y=Number(chartIndex? heart_rate2.value:heart_rate.value);
      break;
      case 'spo2':
        y=Number(chartIndex? blood_oxygen2.value:blood_oxygen.value);
      break;

    default:
      break;
  }
  return y;
}

showChart();

//图形显示
Highcharts.setOptions({
  global: {
      useUTC: false
  }
});

function activeLastPointToolip(chart) {
  var points = chart.series[0].points;
  chart.tooltip.refresh(points[points.length -1]);
}

function showChart(){
 chart = Highcharts.chart('container', {
  chart: {
      type: 'spline',
      marginRight: 10,
      events: {
          load: function () {
              var series = this.series[0],
                  chart = this;
              activeLastPointToolip(chart);
              setInterval(function () {
                  var x = (new Date()).getTime(), // 当前时间
                      y = getChartY(0);//Math.random();          // 随机值
                  series.addPoint([x, y], true, true);
                  activeLastPointToolip(chart);
                  chart.setTitle({text:'L [0] '+display1.options[display1.selectedIndex].value});
              }, 1000);
          }
      }
  },
  title: {
      text: 'L [0] '+display1.options[display1.selectedIndex].value
  },
  xAxis: {
      type: 'datetime',
      tickPixelInterval: 150
  },
  yAxis: {
      title: {
          text: null
      }
  },
  tooltip: {
      formatter: function () {
          return '<b>' + 'L [0] '+display1.options[display1.selectedIndex].value + '</b><br/>' +
              Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) + '<br/>' +
              Highcharts.numberFormat(this.y, 2);
      }
  },
  legend: {
      enabled: false
  },
  series: [{
      name: display1.options[display1.selectedIndex].value+' L[0]',
      data: (function () {
          // 生成随机值
          var data = [],
              time = (new Date()).getTime(),
              i;
          for (i = -19; i <= 0; i += 1) {
              data.push({
                  x: time + i * 1000,
                  y:getChartY(0)// Math.random()
              });
          }
          return data;
      }())
  }]
});


 chart2 = Highcharts.chart('container2', {
  chart: {
      type: 'spline',
      marginRight: 10,
      events: {
          load: function () {
              var series = this.series[0],
                  chart = this;
              activeLastPointToolip(chart);
              setInterval(function () {
                  var x = (new Date()).getTime(), // 当前时间
                      y = getChartY(1);//Math.random();          // 随机值
                  series.addPoint([x, y], true, true);
                  activeLastPointToolip(chart);
                  chart2.setTitle({text:'R [1] '+display2.options[display2.selectedIndex].value});
              }, 1000);
          }
      }
  },
  title: {
      text: 'R [1]  '+display2.options[display2.selectedIndex].value
  },
  xAxis: {
      type: 'datetime',
      tickPixelInterval: 150
  },
  yAxis: {
      title: {
          text: null
      }
  },
  tooltip: {
      formatter: function () {
          return '<b>' + 'R [1]  '+display2.options[display2.selectedIndex].value + '</b><br/>' +
              Highcharts.dateFormat('%Y-%m-%d %H:%M:%S', this.x) + '<br/>' +
              Highcharts.numberFormat(this.y,2);
      }
  },
  legend: {
      enabled: false
  },
  series: [{
      name: display2.options[display2.selectedIndex].value+' R[1]',
      data: (function () {
          // 生成随机值
          var data = [],
              time = (new Date()).getTime(),
              i;
          for (i = -19; i <= 0; i += 1) {
              data.push({
                  x: time + i * 1000,
                  y:getChartY(1)// Math.random()
              });
          }
          return data;
      }())
  }]
});
}