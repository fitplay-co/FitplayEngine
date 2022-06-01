const { SerialPort } = require('serialport');
const { ByteLengthParser } = require('@serialport/parser-byte-length')
const EventEmitter = require('events').EventEmitter;
const m_pnpId="USB\\VID_3030&PID_4001";
let portPath=null;
let port=null;
const m_rev_len=54;

const parseEvent = new EventEmitter();

const CM_HEAD_AND_CHECK_LEN=4;
const revBufMaxLen=1024;
let revBuf = new Uint8Array(revBufMaxLen);
let revBUfLen=0;  //接收数据的长度
let dataLen=0;    //协议里面的数据长度

let platform=process.platform;

let protocol = require('./src/protocol.js');
let webSocketClient= require('./src/websocketClient.js');
//let webSocketServer= require('./src/websocketServer.js');
//let httpServer=require('./src/httpServer.js');
let webClient=new webSocketClient();
//let webServer=new webSocketServer();

// module.exports=function(){

//     this.startSerialPort=()=>{ 
//       //启动扫描串口
//       checkScanning();

//       webClient.websocketConnect();
//     }
//   }


//启动扫描串口
checkScanning();

webClient.websocketConnect(); 

function scanningPort(){
  //Promise approach
  SerialPort.list().then(ports => {
    ports.forEach(function(port) {
      console.log(port.path);
      console.log(port.pnpId);
      console.log(port.manufacturer);

        try {
         console.log("正在扫描串口...");
        } catch (error) {
          console.error("scanningPort error ");
        }
        
        //window
        if(platform=="win32" && port.pnpId.startsWith(m_pnpId)){
          portPath= port.path;
          openSerial();
        }

        //mac
        if(platform=="darwin" && port.manufacturer=="ZEPHYR"){
          portPath= port.path;
          openSerial();
        }
    });
  });
}

function clearRevBuff(){
  revBuf.length=0;
  revBUfLen=0;
  dataLen=0;
}

function opening(){
  port = new SerialPort({ path: portPath, baudRate: 115200,autoOpen:false });

  //webServer.setSerialPort(port);
  webClient.setSerialPort(port);

  console.log("正在打开串口"+portPath);

  port.open((err)=>{
    console.log("isOpen:",portPath);
    if(err){
      console.log("打开串口错误:",portPath);
    }
  });

  const parser = port.pipe(new ByteLengthParser({ length: m_rev_len }));

  parser.on('data',function(data){
   // console.log(data.length);
    if(data.length>revBufMaxLen)
    {
        console.error("");
        return ;
    }

    revBuf.set(data,revBUfLen);
    revBUfLen+=data.length;

   // console.log("======revBuf:",revBuf);

    if(revBuf[0]==0xAA)
    {
      if(dataLen==0 && revBUfLen>=CM_HEAD_AND_CHECK_LEN)
      {
        dataLen=revBuf[2];
      }
    }
    else
    {
      console.error("dislocation ==>",data.length);
      console.log(revBuf);
      clearRevBuff(); 
    }
    
    if(dataLen>CM_HEAD_AND_CHECK_LEN && revBUfLen>=dataLen)
    {
       parseEvent.emit('data',revBuf.slice(0,dataLen),dataLen);
       clearRevBuff();     
    }
   });

   parseEvent.on("data",(buf,len)=>{
     let pro=new protocol();

     pro.parse(buf,len,(error,data)=>{
       if(error)
       {
         console.error(error);
         return;
       }
      
       try {
        webClient.send(data);
       // webServer.send_all(data); 

       } catch (error) {
        console.error(error);
       }

      // console.log("parse======>",data);
     });
   });

}

function checkScanning() {
   if(port==null)
   {
     scanningPort();
     delaySerialOpen(2000);
   }
}
function delaySerialOpen(timeout){
  setTimeout(()=>{
    checkScanning();
  },timeout);
}

function openSerial(){

   opening();

   port.on('close', function () {
    port=null;
    webClient.setSerialPort(port);
    portPath=null;
    console.log('CLOSE');
    parseEvent.removeAllListeners();

    try {
      //mainWindow.webContents.send('update-msg', "串口关闭，准备2s后重新扫描！");
    } catch (error) {
      console.error(error);
    }

    delaySerialOpen(2000);
   });
}