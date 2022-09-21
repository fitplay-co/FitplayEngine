wasm_processor = require('./wasm/wasm_calculation')

var wasm_calculator = {
    process : function(pose, featureConfigs, monitor = false){
        wasm_processor.process(pose, featureConfigs, monitor)
    }
}

module.exports = wasm_calculator;