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
var InputMessage = require('../../../protocol/js/input/input-message').InputMessage
var MessageType = require('../../../protocol/js/input/message-type').MessageType

var wasm_processor = {
    initialized : false ,
    process : function(inputMessage, pose, monitor = false){
        if(!this.initialized) {
            //console.log("do initializing")
            this.instance = new Module.BridgeClass()
            this.initialized = true
            //console.log("initialized")
        }
        
        var actionResult =  this.instance.entry(inputMessage)
        var actionData = new Uint8Array(actionResult)
        if (actionData.length > 0) {
            var actionBuf = new flatbuffers.ByteBuffer(actionData)
            var actionTemp = action.getRootAsAction(actionBuf)
            if (actionTemp.general()) {
                // console.log(actionTemp.general().confidence())
                pose.general_detection = {
                    "confidence" : actionTemp.general().confidence()
                }
            }
            if (actionTemp.walk() || actionTemp.jump() || actionTemp.squat()) {
                pose.action_detection = {
                    "version" : "0.2.0"
                }
                pose.action_detection.walk = {
                    "leftLeg" : actionTemp.walk().leftLeg(),
                    "rightLeg" : actionTemp.walk().rightLeg(),
                    "leftFrequency" : actionTemp.walk().leftFrequency(),
                    "rightFrequency" : actionTemp.walk().rightFrequency(),
                    "leftHipAng" : actionTemp.walk().leftHipAng(),
                    "rightHipAng" : actionTemp.walk().rightHipAng(),
                    "leftStepLength" : actionTemp.walk().leftStepLength(),
                    "rightStepLength" : actionTemp.walk().rightStepLength(),
                    "leftProgress" : actionTemp.walk().leftProgress(),
                    "rightProgress" : actionTemp.walk().rightProgress(),
                    "turn" : actionTemp.walk().turn(),
                    "stepRate" : actionTemp.walk().stepRate(),
                    "stepLen" : actionTemp.walk().stepLen(),
                    "velocity" : actionTemp.walk().velocity()
                }
                pose.action_detection.jump = {
                    "onTheGround" : actionTemp.jump().onTheGround(),
                    "velocity" : actionTemp.jump().velocity()
                }
                pose.action_detection.squat = {
                    "squat" : actionTemp.squat().status()
                }
            }
            if (pose.action_detection && pose.general_detection) {
                pose.monitor = {
                    "m1" : pose.action_detection.jump.onTheGround,
                    "m1Name" : "on_the_ground",
                    "m2" : pose.general_detection.confidence,
                    "m2Name" : "confidence"
                }
            }
            
            // pose.monitor = {
            //     "m1" : pose.keypoints[25].y,
            //     "m1Name" : "leftKnee",
            //     "m2" : pose.keypoints[27].y,
            //     "m2Name" : "leftFoot"
            // }
            if (actionTemp.ground()) {
                pose.ground_location = {
                    "x" : actionTemp.ground().x(),
                    "y" : actionTemp.ground().y(),
                    "z" : actionTemp.ground().z(),
                    "legLength" : actionTemp.ground().legLength(),
                    "tracing" : actionTemp.ground().tracing()
                }
            }
            // console.log(pose.ground_location.z)
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
                pose.fitting.localRotation = []
                pose.fitting.mirrorLocalRotation = []
        
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
                
                for(var i = 0; i<18; i++) {
                    pose.fitting.localRotation.push({
                        "name" : actionTemp.fitting().localRotation(i).name(),
                        "w" : actionTemp.fitting().localRotation(i).w(),
                        "x" : actionTemp.fitting().localRotation(i).x(),
                        "y" : actionTemp.fitting().localRotation(i).y(),
                        "z" : actionTemp.fitting().localRotation(i).z(),
                    })
                }
                for(var i = 0; i<18; i++) {
                    pose.fitting.mirrorLocalRotation.push({
                        "name" : actionTemp.fitting().mirrorLocalRotation(i).name(),
                        "w" : actionTemp.fitting().mirrorLocalRotation(i).w(),
                        "x" : actionTemp.fitting().mirrorLocalRotation(i).x(),
                        "y" : actionTemp.fitting().mirrorLocalRotation(i).y(),
                        "z" : actionTemp.fitting().mirrorLocalRotation(i).z(),
                    })
                }
                // console.log(pose.fitting)
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
    }, 
}

module.exports = wasm_processor;