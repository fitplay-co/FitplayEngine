// var Module = require('../../wasm_modules/processor.js');
var NewModule = require('../../wasm_modules/bridge-class.js');

var flatbuffers = require('flatbuffers');
var Point = require('../../../../common/protocol/js/pose-data/point').Point
var Pose = require('../../../../common/protocol/js/pose-data/pose').Pose;
var fs = require('fs');
const { z } = require('../midware_gaze_tracking.js');
const { type } = require('os');
var action = require('../../../../common/protocol/js/action-data/action').Action
var FeatureConfig = require('../../../../common/protocol/js/feature-configs/feature-config').FeatureConfig
var FeatureConfigList = require('../../../../common/protocol/js/feature-configs/feature-config-list').FeatureConfigList
var InputMessage = require('../../../../common/protocol/js/input/input-message').InputMessage
var OutputMessage = require('../../../../common/protocol/js/output/output-message').OutputMessage
var MessageType = require('../../../../common/protocol/js/input/message-type').MessageType

var wasm_processor = {
    initialized : false ,
    process : function(inputMessage, pose, monitor = false){
        if(!this.initialized) {
            //console.log("do initializing")
            // this.instance = new Module.BridgeClass()
            NewModule.print = function(text) {
                if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
                console.warn(text);
            }
            NewModule.printErr =  function(text) {
                if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
                console.error(text);
            },
            this.newModeule = new NewModule.BridgeClass()
            console.log("module loaded")
            this.initialized = true
            //console.log("initialized")
        }
        
        // var actionResult =  this.instance.entry(inputMessage)
        var midwareResult = this.newModeule.entry(inputMessage)

        // console.log("got new result")
        var midwareResultUint8Array = new Uint8Array(midwareResult)
        if (midwareResultUint8Array.length > 0) {
            pose.flatbuffersData = midwareResultUint8Array
            pose.json = this.newModeule.json()
        }

        this.newModeule.release()
        
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