<template>
  <div class="about">
    <h1>Fitplay Debugging Tools </h1>
  </div>
  <div>Keypoint
        <select id="keypoints">
            <option value="0" class="active">nose</option>
            <option value="11">left_shoulder</option>
            <option value="12">right_shoulder</option>
            <option value="13">left_elbow</option>
            <option value="14">right_elbow</option>
            <option value="15">left_wrist</option>
            <option value="16">right_wrist</option>
            <option value="19">left_index</option>
            <option value="20">right_index</option>
            <option value="23">left_hip</option>
            <option value="24">right_hip</option>
            <option value="25">left_knee</option>
            <option value="26">right_knee</option>
            <option value="27">left_ankle</option>
            <option value="28">right_ankle</option>
            <option value="31">left_foot_index</option>
            <option value="32">right_foot_index</option>
        </select> 
    </div>
    <div>Keypoint Type
        <select id="keypoints_type">
            <option value="1" class="active">2D</option>
            <option value="2">3D</option>
        </select> 
    </div>
    <div>Keypoint Data
        <select id="keypoints_data">
            <option value="1" class="active">x</option>
            <option value="2">y</option>
            <option value="3">z</option>
            <option value="4">score</option>
        </select> 
    </div>
    <div id="chart1" style="width: 800px;height:400px;"></div>
    <div>Ground Location
        <select id="ground_location">
            <option value="1" class="active">x</option>
            <option value="2">y</option>
            <option value="3">z</option>
        </select> 
    </div>
    <div id="chart2" style="width: 800px;height:400px;"></div>
</template>

<style>
@media (min-width: 1024px) {
  .about {
    min-height: 100vh;
    display: flex;
    align-items: center;
  }
}
</style>
<script lang="ts">
import { flatbuffersMessage } from '@/client/MonitorClient';
import * as echarts from 'echarts';
import { onMounted } from 'vue';
import { InputMessage, Control, Pose} from "../protocol/ts/inputMessage"
import { OutputMessage} from "../protocol/ts/outputMessage"
import { Client } from "../protocol/ts/inputMessage"
import { ByteBuffer } from '../protocol/ts/flatbuffers/flatbuffers'; // located in node_modules
import { Builder } from '../protocol/ts/flatbuffers/builder'
import { MessageType } from '../protocol/ts/inputMessage'

export default {
  setup() {
    onMounted(()=>{
      // const chart1 = echarts.init(document.getElementById('chart1') as HTMLDivElement);
      // chart1.setOption(option)
      // const chart2 = echarts.init(document.getElementById('chart2') as HTMLDivElement);
      // chart2.setOption(option2)
      var arr = []
for(let i = 1,len=100;i<=len;i++){arr.push(i)}
var num1 = new Array(100).fill(0);
var num2 = new Array(100).fill(0);
var option = {
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
};
var option2 = {
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
      data: num2,
      type: "line"
    }]
};
const chart1 = echarts.init(document.getElementById('chart1') as HTMLDivElement);
chart1.setOption(option)
const chart2 = echarts.init(document.getElementById('chart2') as HTMLDivElement);
chart2.setOption(option2)
let url = 'ws://192.168.50.106:8181'
var ws : WebSocket;
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
  ws.binaryType = "arraybuffer"
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
    addFlatData(e.data);
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
  setTimeout(() => {
      ws_send(flatbuffersMessage(appClientFlatMessage))
  }, 1000);
    setTimeout(() => {
      ws_send(flatbuffersMessage(actionDetectionSubscribe))
  }, 1000);
    setTimeout(() => {
      ws_send(flatbuffersMessage(generalDetectionSubscribe))
  }, 1000);
    setTimeout(() => {
      ws_send(flatbuffersMessage(groundLocationSubscribe))
  }, 1000);
    setTimeout(() => {
      ws_send(flatbuffersMessage(gazeSubscribe))
  }, 1000);
    setTimeout(() => {
      ws_send(flatbuffersMessage(fittingSubscribe))
  }, 1000);
  }

function ws_send(data:any){
if(ws.readyState==WebSocket.OPEN)
{
  // console.log(data);
  ws.send(data);
}
}

function addFlatData(inputData:any){
    var select = (document.getElementById('keypoints') as HTMLSelectElement).options[(document.getElementById('keypoints') as HTMLSelectElement).selectedIndex];
    var value = Number(select.value);
    var text = select.text;
    var select_type = Number((document.getElementById('keypoints_type') as HTMLSelectElement).options[(document.getElementById('keypoints_type') as HTMLSelectElement).selectedIndex].value);
    var select_data = Number((document.getElementById('keypoints_data') as HTMLSelectElement).options[(document.getElementById('keypoints_data') as HTMLSelectElement).selectedIndex].value);

    var ground_location_data = Number((document.getElementById('ground_location') as HTMLSelectElement).options[(document.getElementById('ground_location') as HTMLSelectElement).selectedIndex].value);
    const z = new Uint8Array(inputData);
    var outputMessage = OutputMessage.getRootAsOutputMessage(new ByteBuffer(z))
    var pose = outputMessage.pose()!;
    var detectionResult = outputMessage.detectionResult()!;
    var groundLocation = detectionResult.ground()!;

    var m1 : Number | undefined;
    var m2 : Number | undefined;

    if (select_data == 1 && select_type == 1) m1 = pose.keypoints(value)?.x();
    if (select_data == 2 && select_type == 1) m1 = pose.keypoints(value)?.y();
    if (select_data == 3 && select_type == 1) m1 = pose.keypoints(value)?.z();

    if (select_data == 1 && select_type == 2) m1 = pose.keypoints3D(value)?.x();
    if (select_data == 2 && select_type == 2) m1 = pose.keypoints3D(value)?.y();
    if (select_data == 3 && select_type == 2) m1 = pose.keypoints3D(value)?.z();

    if(ground_location_data == 1) m2 = groundLocation.x();
    if(ground_location_data == 2) m2 = groundLocation.y();
    if(ground_location_data == 3) m2 = groundLocation.z();

    num1.push(m1)
    num1.shift()

    num2.push(m2)
    num2.shift()
    console.log(m2)


    // const chart1 = echarts.init(document.getElementById('chart1') as HTMLDivElement)
    // const chart2 = echarts.init(document.getElementById('chart2') as HTMLDivElement)
    chart1.setOption({
        title: {
          subtext: text
        },
        yAxis: {
          type: "value"
        },
        series: [{
          data: num1
        }]
    });
    chart2.setOption({
        title: {
          subtext: "Ground Location"
        },
        yAxis: {
          type: "value"
        },
        series: [{
          data: num2
        }]
    });
}
    });
  },

  method: {

  }
  
}

// var chart1 : echarts.ECharts;

</script>
