var Module = require('./dist/sample.js');

Module.onRuntimeInitialized = function() {
    var vec = Module.returnVectorData();
    var vectorSize = vec.size();
    var obj = {"happy": true, "pi": 3.141};
    var jsonstr = JSON.stringify(obj);
    console.log(jsonstr);
    console.log(Module.jsonFunc(jsonstr));
    console.log(Module.lerp(1, 2, 0.5));
    console.log(Module.testFunc("test"));
    console.log(vectorSize);
}