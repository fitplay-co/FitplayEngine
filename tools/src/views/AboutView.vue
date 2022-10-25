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
import { ClientInstance } from '@/client/MonitorClient';
import * as echarts from 'echarts';
import { onMounted } from 'vue';

export default {
  setup() {
    onMounted(()=>{
      const chart1 = echarts.init(document.getElementById('chart1') as HTMLDivElement);
      chart1.setOption(option)
    });
  }
}
// ClientInstance.sayHello();
// let url = 'ws://localhost:8181/'
// let ws = new WebSocket(url)

var arr = []
for(let i = 1,len=100;i<=len;i++){arr.push(i)}
var num1 = new Array(100).fill(0);
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
</script>
