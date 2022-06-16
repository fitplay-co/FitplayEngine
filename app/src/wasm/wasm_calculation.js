var Module = require('../../wasm_modules/processor.js');
var flatbuffers = require('flatbuffers');
var Point = require('../../../protocol/js/pose-data/point').Point
var Pose = require('../../../protocol/js/pose-data/pose').Pose;
var fs = require('fs');
const { z } = require('../midware_gaze_tracking.js');
const { type } = require('os');
var action = require('../../../protocol/js/action-data/action').Action
var FeatureConfig = require('../../../protocol/js/feature-configs/feature-config').FeatureConfig
var FeatureConfigList = require('../../../protocol/js/feature-configs/feature-config-list').FeatureConfigList

var wasm_processor = {
    initialized : false ,
    process : function(pose, featureConfigs, monitor = false){
        if(!this.initialized) {
            //console.log("do initializing")
            this.instance = new Module.BridgeClass(10,"hello")
            this.initialized = true
            //console.log("initialized")
        }
        
        var builder = new flatbuffers.Builder(1024)     
        var actionName = builder.createString("aciton_detection, gaze_tracking, ground_location, fitting")   
        var keyPoints = new Array(33)
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
        //console.log(keyPoints)
        var KeyPoints = Pose.createKeypointsVector(builder,keyPoints)

        var keypoints3d = new Array(33)
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
        //console.log(KeyPoints)
        var keyPoints3d = Pose.createKeypoints3DVector(builder,keypoints3d)

        Pose.startPose(builder)
        Pose.addAction(builder, actionName)
        Pose.addKeypoints(builder,KeyPoints)
        Pose.addKeypoints3D(builder,keyPoints3d)
        var res = Pose.endPose(builder)
        builder.finish(res)

        // build feature configs
        var configBuilder = new flatbuffers.Builder(1024)
        var featureConfigArray = new Array(featureConfigs.length)
        for (var i = 0; i < featureConfigArray.length; i++) {
            var featureId = configBuilder.createString(featureConfigs[i].featureId)
            var actionString = configBuilder.createString(featureConfigs[i].action)
            var data = configBuilder.createString(featureConfigs[i].data)
            FeatureConfig.startFeatureConfig(configBuilder)
            FeatureConfig.addFeatureId(configBuilder, featureId)
            FeatureConfig.addEnable(configBuilder, featureConfigs[i].enable)
            FeatureConfig.addAction(configBuilder, actionString)
            FeatureConfig.addData(configBuilder, data)
            var config = FeatureConfig.endFeatureConfig(configBuilder)
            //console.log(point)
            featureConfigArray[i] = config
        }
        var featureConfigsFlatbuffer = FeatureConfigList.createConfigsVector(configBuilder, featureConfigArray)
        FeatureConfigList.startFeatureConfigList(configBuilder)
        FeatureConfigList.addConfigs(configBuilder, featureConfigsFlatbuffer)
        var configListBuffer = FeatureConfigList.endFeatureConfigList(configBuilder)
        configBuilder.finish(configListBuffer)

        var actionResult =  this.instance.entry(builder.asUint8Array(), configBuilder.asUint8Array())
        var actionData = new Uint8Array(actionResult)
        var actionBuf = new flatbuffers.ByteBuffer(actionData)
        var actionTemp = action.getRootAsAction(actionBuf)
        if (actionTemp.walk() || actionTemp.jump()) {
            pose.action_detection = {}
            pose.action_detection.walk = {
                 "legUp" : actionTemp.walk().legUp(),
                 "frequency" : actionTemp.walk().frequency(),
                 "strength" : actionTemp.walk().strength()
            }
            pose.action_detection.jump = {
                "up" : actionTemp.jump().up(),
                "strength" : actionTemp.jump().strength()
            }
        }
        if (actionTemp.ground()) {
            pose.ground_location = {
                "x" : actionTemp.ground().x(),
                "y" : actionTemp.ground().y(),
                "z" : actionTemp.ground().z(),
                "legLength" : actionTemp.ground().legLength(),
                "tracing" : actionTemp.ground().tracing()
            }
        }
        if (actionTemp.gaze()) {
            pose.gaze_tracking = {
                "x" : actionTemp.gaze().x(),
                "y" : actionTemp.gaze().y(),
                "z" : actionTemp.gaze().z()
            }
        }
        if (actionTemp.fitting()) {
            pose.fitting = {}
            pose.fitting.rotation = []
            pose.fitting.mirrorRotation = []
    
            for(var i = 0; i<18; i++) {
                pose.fitting.rotation.push({
                    "name" : actionTemp.fitting().rotation(i).name(),
                    "w" : actionTemp.fitting().rotation(i).w(),
                    "x" : actionTemp.fitting().rotation(i).x(),
                    "y" : actionTemp.fitting().rotation(i).y(),
                    "z" : actionTemp.fitting().rotation(i).z(),
                })
            }
    
            for(var i = 0; i<18; i++) {
                pose.fitting.mirrorRotation.push({
                    "name" : actionTemp.fitting().mirrorRotation(i).name(),
                    "w" : actionTemp.fitting().mirrorRotation(i).w(),
                    "x" : actionTemp.fitting().mirrorRotation(i).x(),
                    "y" : actionTemp.fitting().mirrorRotation(i).y(),
                    "z" : actionTemp.fitting().mirrorRotation(i).z(),
                })
            }
        }
        this.instance.release()
        
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

        //var result = this.instance.jsonFunc(builder.asUint8Array())

        
        
       
        // console.log(result)
        // var data = new Uint8Array(result)

        // // console.log("result length:"+result.length)
        // // for (let i = 0; i < result.length; i++) {
        // //     data[i] = result.charAt(i)
        // // }
        // var buf = new flatbuffers.ByteBuffer(data)
        // var temp = Pose.getRootAsPose(buf)
        // // var temp2 = temp.keypoints3DLength
        // //console.log(temp.keypoints3D(1).x())
        // this.instance.release()

        // result = JSON.parse(this.instance.jsonFunc(JSON.stringify(pose)))

        // pose.fitting = result.fitting
        //console.log(this.instance.jsonFunc(jsonstr))

        // builder = new flatbuffers.Builder(1024)
        // keyPoints = new Array(3)
        // var name = builder.createString('nose')
        // Point.startPoint(builder)
        // Point.addX(builder, 1)
        // Point.addY(builder, 1)
        // Point.addZ(builder, 1)
        // Point.addScore(builder, 1)
        // Point.addName(builder,name)
        // var point = Point.endPoint(builder)
        // //console.log(point)
        // keyPoints[0] = point

        // name = builder.createString('arm')
        // Point.startPoint(builder)
        // Point.addX(builder, 1)
        // Point.addY(builder, 1)
        // Point.addZ(builder, 1)
        // Point.addScore(builder, 1)
        // Point.addName(builder,name)
        // point = Point.endPoint(builder)
        // //console.log(point)
        // keyPoints[1] = point

        // name = builder.createString('leg')
        // Point.startPoint(builder)
        // Point.addX(builder, 1)
        // Point.addY(builder, 1)
        // Point.addZ(builder, 1)
        // Point.addScore(builder, 1)
        // Point.addName(builder,name)
        // point = Point.endPoint(builder)
        // //console.log(point)
        // keyPoints[2] = point

        // KeyPoints = Pose.createKeypointsVector(builder,keyPoints)
        // //console.log(KeyPoints)
        // keyPoints3d = Pose.createKeypoints3DVector(builder,keyPoints)
        // Pose.startPose(builder)
        // Pose.addKeypoints(builder,KeyPoints)
        // Pose.addKeypoints3D(builder,keyPoints3d)
        // var poseT = Pose.endPose(builder)
        // builder.finish(poseT)
        // console.log("after: "+builder.asUint8Array())
    }, 
}

module.exports = wasm_processor;