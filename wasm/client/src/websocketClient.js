const WebSocket = require('ws');
const url='ws://127.0.0.1:8181';
ws=null;
isRconnect=false;
let m_port=null;
let protocol = require('./protocol.js');

module.exports=function(){

  this.websocketConnect=()=>{ 
    websocketCreate();
  }

  this.setSerialPort=(port)=>{
    m_port=port;
  }

 this.send=((data)=>{

  if(ws.readyState==WebSocket.OPEN)
  {
    //console.log(data);
    ws.send(data);
  }
});
}

function websocketCreate(){
  try {
    // 打开一个 web socket
     ws = new WebSocket(url);
    
     ws_init();
  
    } catch (error) {
     // console.error("websocket_connect error!");
      websocketReconnect();
    }
}

function websocketReconnect(){
   // console.log("websocketReconnect "+isRconnect);
    if(isRconnect==true)
    {
      return;
    }
  
    isRconnect=true;
   // console.log("will reconnect "+url);
    setTimeout(()=>{
      websocketCreate();
      isRconnect=false;
    },2000);
  }
  
  function ws_init(){
    ws.onopen = () => {
      console.log("onopen");

      var appClientMessage =  {
        "type" : "application_client",
        "id": "sensor_client"
      }
    
      setTimeout(() => {
        ws.send(JSON.stringify(appClientMessage))
      }, 1000);
    }
  
    ws.onclose = function () {
     // console.log('onclose');
      websocketReconnect();
    };
  
    ws.onerror = (error) => {
     // console.log("WebSocket onerror "+error);
      websocketReconnect();
    }
    
    ws.onmessage = (e) => {
      let message=e.data;
      // let jsonObj=JSON.parse(message);


      // if(jsonObj.sensor_type !="vibration")
      {
        //console.log("[websocketClient] is not support type:%s",jsonObj.type);
        return ;
      }

      console.log('[websocketClient] Received message =>'+message);
      let pro=new protocol();

      try {
          //封装协议
          pro.pack(message,(uint8arr)=>{
            console.log(uint8arr);
            if(m_port)
            {
              m_port.write(uint8arr);
            }
          });
      } catch (error) {
        // console.log("onmessage "+error);
      }
    }
  }