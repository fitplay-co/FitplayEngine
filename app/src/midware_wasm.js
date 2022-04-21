wasm_processor = require('./wasm/wasm_calculation')

var wasm_calculator = {
    process : function(pose, monitor = false){
        wasm_processor.process(pose, monitor)
    }
}

module.exports = wasm_calculator;