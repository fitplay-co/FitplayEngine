import { createApp } from "vue";
import App from "./App.vue";
import router from "./router";
import * as echarts from 'echarts'
import {ClientInstance} from "./client/MonitorClient";

import "./assets/main.css";

ClientInstance.sayHello();

const app = createApp(App);

app.config.globalProperties.$echarts = echarts
app.use(router);

app.mount("#app");
