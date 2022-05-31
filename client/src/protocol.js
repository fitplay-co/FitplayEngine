const CMD_OFFSET=1;
const VERSION_OFFSET=3;
const LEN_OFFSET=2;
const DEVICEID_OFFSET=4;

//cmd 类型
const CMD_IMU=0;
const CMD_INPUT=1;
const CMD_DEVICEINFO=2;
const CMD_VIRBACTION=3;

const CMD_VIRBACTION_LEN=12;

const START_CODE=0xAA;

let imuObj={
  "type": "sensor_frame",
  "sensor_type": "imu",
  "version": "0.0.0",
  "device_id": 255,
  "timestamp": 0,
  "seq": 0,
  "accelerometer": 
  {
      "x": 0.00000000,
      "y": 0.00000000,
      "z": 0.00000000
  },
  "gyroscope": 
  {
      "x": 0.00000000,
      "y": 0.00000000,
      "z": 0.00000000
  },
  "magnetometer": 
  {
      "x": 0.00000000,
      "y": 0.00000000,
      "z": 0.00000000
  },
  "quaternions": 
  {
      "x": 0.00000000,
      "y": 0.00000000,
      "z": 0.00000000,
      "w": 0.00000000
  }
};

let inputObj={
  "type": "input",
  "version": "0.0.0",
  "device_id": 255,
  "timestamp": 0,
  "keys": {
      "key_A": 0,
      "key_B": 0,
      "key_menu": 0
  },
  "linear_key": {
      "L1": 0,
      "L2": 0
  },
  "joystick": {
      "x": 0,
      "y": 0,
      "key": 0
  },
  "heart_rate": 0,
  "blood_oxygen": 0
};

let deviceObj={
  "type": "device_info",
  "version": "0.0.0",
  "device_id": 0,
  "key_size": 0,
  "imu_type": 0,
  "linear_key_size": 0,
  "joystick": 0,
  "vibration": 0,
  "vibration_strength": 0,
  "bat_percent": 0
};

module.exports=function(){
    this.uint8buff;
    this.buflen; 
    this.jsObjDict={0:imuObj,
                    1:inputObj,
                    2:deviceObj};

    
    this.checkSum=(u8Array,len)=>{
        let u8a = new Uint8Array(1);
    
        for(let i=0;i<len-1;i++)
        {
          u8a[0]+=u8Array[i];
        }
    
        return u8a[0];
    }
    
   this.pack=((jsObj,callback)=>{
      let jsonObj=JSON.parse(jsObj);
      console.log("pack jsonObj.cmd:%d",jsonObj.cmd);

      switch(jsonObj.cmd){
        case CMD_VIRBACTION:
          {
            let uint8arr=new Uint8Array(CMD_VIRBACTION_LEN);
            let dataview=new DataView(uint8arr.buffer);
            let timestamp= Math.round(new Date().getTime()/1000);

            console.log(timestamp);

            dataview.setUint8(0,START_CODE);
            dataview.setUint8(1,jsonObj.cmd);
            dataview.setUint8(2,CMD_VIRBACTION_LEN);
            dataview.setUint8(3,0);
            dataview.setUint8(4,jsonObj.device_id);
            dataview.setUint32(5,timestamp,true);
            dataview.setUint8(9,jsonObj.vibration_type);
            dataview.setUint8(10,jsonObj.strength);
            dataview.setUint8(11,this.checkSum(uint8arr,CMD_VIRBACTION_LEN));

            callback(uint8arr);
          }
          break;

          default:
            console.log("%d no this cmd support!",jsonObj.cmd);
        
      }

   });

   //解释二进制协议 ，输出json协议格式数据
   this.parse=(uint8buff,buflen,callback)=>{
        this.uint8buff=uint8buff;
        this.buflen=buflen; 

        let err=NaN;
        let datalen=this.uint8buff[LEN_OFFSET];
        let checksum=this.uint8buff[this.buflen-1];

       // console.log("this.uint8buff",this.uint8buff);
        if(this.uint8buff[0]!=START_CODE)
        {
             err="start code is not 0xAA";
             callback(err,NaN);
             return ;
        }

        if(this.buflen!=datalen)
        {
           err=`datalen(protocol data len):${datalen} is not equal buflen(real buf len):${this.buflen}`;
           callback(err,NaN);
            return ;
        }

        let bufChecksum=this.checkSum(this.uint8buff,this.buflen);

        if(checksum!=bufChecksum)
        {
          err=`checksum(read buf last value):${checksum} is not equal bufChecksum(accoding buf calculate the checksum):${bufChecksum}`;
          callback(err,NaN);
          return ;
        }
        
        let dataview=new DataView(this.uint8buff.buffer);
        let cmd=dataview.getUint8(CMD_OFFSET);

        let jsOjb=this.jsObjDict[cmd];

        try {
          jsOjb.version=dataview.getUint8(VERSION_OFFSET);
          jsOjb.device_id=dataview.getUint8(DEVICEID_OFFSET);
        } catch (error) {
          console.error("not support protocol: "+this.uint8buff);
        }

        if(cmd==CMD_INPUT)
        {
          let jsOjbImu=this.jsObjDict[CMD_IMU];

          jsOjbImu.version=dataview.getUint8(VERSION_OFFSET);
          jsOjbImu.device_id=dataview.getUint8(DEVICEID_OFFSET);


          jsOjbImu.timestamp=dataview.getUint32(5,true);
          jsOjbImu.seq=dataview.getUint32(9,true);
          jsOjbImu.accelerometer.x=(dataview.getInt16(13,true)/32768*16).toFixed(7);
          jsOjbImu.accelerometer.y=(dataview.getInt16(15,true)/32768*16).toFixed(7);
          jsOjbImu.accelerometer.z=(dataview.getInt16(17,true)/32768*16).toFixed(7);

          jsOjbImu.gyroscope.x=(dataview.getInt16(19,true)/32768*2000).toFixed(7);
          jsOjbImu.gyroscope.y=(dataview.getInt16(21,true)/32768*2000).toFixed(7);
          jsOjbImu.gyroscope.z=(dataview.getInt16(23,true)/32768*2000).toFixed(7);  
          
          jsOjbImu.magnetometer.x=dataview.getUint16(25,true)<<4;
          jsOjbImu.magnetometer.y=dataview.getUint16(27,true)<<4;
          jsOjbImu.magnetometer.z=dataview.getUint16(29,true)<<4;    

    
          jsOjbImu.quaternions.x=(dataview.getInt16(31,true)/32768).toFixed(7);
          jsOjbImu.quaternions.y=(dataview.getInt16(33,true)/32768).toFixed(7);
          jsOjbImu.quaternions.z=(dataview.getInt16(35,true)/32768).toFixed(7);  
          jsOjbImu.quaternions.w=(dataview.getInt16(37,true)/32768).toFixed(7);  


          callback(err,JSON.stringify(jsOjbImu));

   
          let jsOjbInput=this.jsObjDict[CMD_INPUT];
          
       //   console.log(uint8buff);

          jsOjbInput.version=dataview.getUint8(VERSION_OFFSET);
          jsOjbInput.device_id=dataview.getUint8(DEVICEID_OFFSET);
          jsOjbInput.timestamp=dataview.getUint32(5,true);

          jsOjbInput.keys.key_A=dataview.getUint8(39);
          jsOjbInput.keys.key_B=dataview.getUint8(40);
          jsOjbInput.keys.key_menu=dataview.getUint8(41);
          jsOjbInput.linear_key.L1=dataview.getUint16(42,true);
          jsOjbInput.linear_key.L2=dataview.getUint16(44,true);
          jsOjbInput.joystick.x=dataview.getUint16(46,true);
          jsOjbInput.joystick.y=dataview.getUint16(48,true);   
          jsOjbInput.joystick.key=dataview.getUint8(50);   
          jsOjbInput.heart_rate=dataview.getUint8(51);   
          jsOjbInput.blood_oxygen=dataview.getUint8(52);   

          callback(err,JSON.stringify(jsOjbInput));
          
        }
        

        if(cmd==CMD_DEVICEINFO)
        {
          jsOjb.key_size=dataview.getUint8(5);
          jsOjb.imu_type=dataview.getUint8(6);
          jsOjb.linear_key_size=dataview.getUint8(7);
          jsOjb.joystick=dataview.getUint8(8);
          jsOjb.vibration=dataview.getUint8(9);
          jsOjb.vibration_strength=dataview.getUint8(10);
          jsOjb.bat_percent=dataview.getUint8(11);
          
          callback(err,JSON.stringify(jsOjb));
        }


        //console.log("parse",jsOjb);
       // callback(err,JSON.stringify(jsOjb));     
    }
}