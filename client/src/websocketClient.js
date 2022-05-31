const WebSocket = require('ws');
const url='ws://127.0.0.1:8181';
ws=null;
isRconnect=false;

module.exports=function(){

  this.websocketConnect=()=>{ 
    websocketCreate();
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
      console.log('[websocketClient] Received message =>'+message);
      let jsonObj=JSON.parse(message);

      if(jsonObj.type==="vibration")
      {
        console.log("[websocketClient] is not support type:%s",jsonObj.type);
        return ;
      }

      let pro=new protocol();
      try {
          //封装协议
          pro.pack(message,(uint8arr)=>{
            console.log(uint8arr);
            if(port)
            {
              port.write(uint8arr);
            }
          });
      } catch (error) {
        console.log("onmessage "+error);
      }
    }
  }