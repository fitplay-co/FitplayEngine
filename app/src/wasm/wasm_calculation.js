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
        var jsonstr = JSON.stringify(pose)
        //console.log(this.instance.jsonFunc(jsonstr))
        //console.log("process")
    }, 
}

module.exports = wasm_processor;