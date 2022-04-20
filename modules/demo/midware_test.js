var Module = require('./build/mytest.js');

Module.onRuntimeInitialized = function() {
    console.log(Module.getText());
}