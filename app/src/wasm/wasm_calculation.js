var Module = require('../../wasm_modules/processor.js');
var flatbuffers = require('./flatbuffers/flatbuffers.js');
var TestC = require('./flatbuffers/test-c.js').TestC;

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
        var name = builder.createString('testName')
        TestC.startTestC(builder)
        TestC.addName(builder, name)
        var testc = TestC.endTestC(builder)
        builder.finish(testc)
        var result = this.instance.jsonFunc(builder.asUint8Array())
        console.log('wasm bridge result:'+result)
        // result = JSON.parse(this.instance.jsonFunc(JSON.stringify(pose)))
        // pose.fitting = result.fitting
        //console.log(this.instance.jsonFunc(jsonstr))
    }, 
}

module.exports = wasm_processor;