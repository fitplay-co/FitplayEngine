var Module = require('../../wasm_modules/processor.js');
var flatbuffers = require('./flatbuffers/flatbuffers.js');
var Point = require('./flatbuffers/point').Point
var Pose = require('./flatbuffers/pose.js').Pose;
var test = require('./test')
var fs = require('fs')
var wasm_processor = {
    initialized : false ,
    process : function(pose, monitor = false){
        if(!this.initialized) {
            //console.log("do initializing")
            this.instance = new Module.BridgeClass(10,"hello")
            this.initialized = true
            //console.log("initialized")
        }
        var builder = new flatbuffers.Builder(1024)
        var keyPoints = new Array(33)
        var keypoints3d = new Array(33)
        for (var i = 0;i<33;i++){
            var name = builder.createString(pose.keypoints[i].name)
            Point.startPoint(builder)
            Point.addX(builder,pose.keypoints[i].x)
            Point.addY(builder,pose.keypoints[i].y)
            Point.addZ(builder,pose.keypoints[i].z)
            Point.addScore(builder,pose.keypoints[i].score)
            Point.addName(builder,name)
            var point = Point.endPoint(builder)
            //console.log(point)
            keyPoints[i] = point
        }
        for (var i = 0;i<33;i++){
            var name = builder.createString(pose.keypoints3D[i].name)
            Point.startPoint(builder)
            Point.addX(builder,pose.keypoints3D[i].x)
            Point.addY(builder,pose.keypoints3D[i].y)
            Point.addZ(builder,pose.keypoints3D[i].z)
            Point.addScore(builder,pose.keypoints3D[i].score)
            Point.addName(builder,name)
            var point = Point.endPoint(builder)
            //console.log(point)
            keypoints3d[i] = point
        }
        //console.log(keyPoints)
        var KeyPoints = Pose.createKeypointsVector(builder,keyPoints)
        //console.log(KeyPoints)
        var keyPoints3d = Pose.createKeypoints3DVector(builder,keypoints3d)
       Pose.startPose(builder)
       Pose.addKeypoints(builder,KeyPoints)
       Pose.addKeypoints3D(builder,keyPoints3d)
       var res = Pose.endPose(builder)
        builder.finish(res)
        
        // var bbb = builder.asUint8Array()
        // fs.writeFileSync('pose.bin',bbb,'binary')
        // // function stringToUint8Array(str){
        //     var arr = [];
        //     for (var i = 0, j = str.length; i < j; ++i) {
        //       arr.push(str.charCodeAt(i));
        //     }
           
        //     var tmpUint8Array = new Uint8Array(arr);
        //     return tmpUint8Array
        //   }
        function toUint8Arr(str) {
            const buffer = [];
            for (let i of str) {
                const _code = i.charCodeAt(0);
                if (_code < 0x80) {
                    buffer.push(_code);
                } else if (_code < 0x800) {
                    buffer.push(0xc0 + (_code >> 6));
                    buffer.push(0x80 + (_code & 0x3f));
                } else if (_code < 0x10000) {
                    buffer.push(0xe0 + (_code >> 12));
                    buffer.push(0x80 + (_code >> 6 & 0x3f));
                    buffer.push(0x80 + (_code & 0x3f));
                }
            }
            return Uint8Array.from(buffer);
}
        //test.process()

        var result = this.instance.jsonFunc(builder.asUint8Array())
        
        function ltrim(s){     
            return s.replace(/(^\s*)/, "");  
         }   
         //去右空格;   
        function rtrim(s){   
          return s.replace(/(\s*$)/, "");  
        }   
         //去左右空格;   
         function trim(s){  
           s.replace(/(^\s*)|(\s*$)/, "");  
          return rtrim(ltrim(s));   
         
         }   
        //var data = toUint8Arr(result)
        // //var data = toUint8Arr(result)
        var buf = new flatbuffers.ByteBuffer(result)
        // //console.log(stringToUint8Array(result))
        var test = Pose.getRootAsPose(buf)
        console.dir(test.bb.bytes_,{'maxArrayLength': null})
        var key = test.keypoints(1).x()
        // //var x = key.x()
        // //var poi = Point.getRootAsPoint(buf)
        console.log(key)
        //console.log(name)
        //var bytes = new Uint8Array(ReadableStream(result))
        //var buf = new flatbuffers.ByteBuffer(bytes)
        //var test = Pose.getRootAsPose(buf)
        //console.log(test.keypoints)
        //pose.fitting = result.fitting
        //var result = this.instance.jsonFunc(builder.asUint8Array())
        //console.log('wasm bridge result:'+result)
        // result = JSON.parse(this.instance.jsonFunc(JSON.stringify(res)))
        // pose.fitting = result.fitting
        //console.log(this.instance.jsonFunc(jsonstr))
    }, 
}

module.exports = wasm_processor;