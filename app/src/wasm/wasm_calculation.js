var Module = require('../../wasm_modules/processor.js');

var wasm_processor = {
    initialized : false ,
    process : function(pose, monitor = false){
        if(!this.initialized) {
            //console.log("do initializing")
            this.instance = new Module.BridgeClass(10,"hello")
            this.initialized = true
            //console.log("initialized")
        }
        result = JSON.parse(this.instance.jsonFunc(JSON.stringify(pose)))
        pose.fitting = result.fitting
        //console.log(this.instance.jsonFunc(jsonstr))
    }, 
}

module.exports = wasm_processor;