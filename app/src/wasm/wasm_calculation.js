var Module = require('../../wasm_modules/processor.js');
var flatbuffers = require('./flatbuffers/flatbuffers.js');
var Point = require('./flatbuffers/point').Point
var Pose = require('./flatbuffers/pose.js').Pose;

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
        // var result = JSON.parse(this.instance.jsonFunc(JSON.stringify(res)))
        // pose.fitting = result.fitting
        var result = this.instance.jsonFunc(builder.asUint8Array())
        console.log(result)
        var data = new Uint8Array(result)
        var buf = new flatbuffers.ByteBuffer(data)
        var temp = Pose.getRootAsPose(buf)
        var temp2 = temp.keypoints3DLength
        console.log(temp.keypoints3D(1).x())
        // result = JSON.parse(this.instance.jsonFunc(JSON.stringify(pose)))
        // pose.fitting = result.fitting
        //console.log(this.instance.jsonFunc(jsonstr))
    }, 
}

module.exports = wasm_processor;