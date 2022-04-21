var Module = require('../../wasm_modules/processor.js');

var wasm_processor = {
    process : function(pose, monitor = false){
        Module.onRuntimeInitialized = function() {
            var jsonstr = JSON.stringify(pose);
            str = Module.jsonFunc(jsonstr);
            pose = JSON.parse(str);
        }
    }, 
}

module.exports = wasm_processor;