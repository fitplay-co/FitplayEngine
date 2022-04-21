var Module = require('../../../modules/dist/sample.js');
var mockData = require('../test/mock_pose_data');

var obj = mockData[0];
var jsonstr = JSON.stringify(obj);
console.log(jsonstr);
Module.addOnPostRun(function() { 
    var instance = new Module["MyClass"](10, "hello");
    instance["incrementX"]();
    var temp1 = instance["jsonFunc"](jsonstr);
    instance["x"]; // 11
    instance["x"] = 20; // 20
    var temp2 = Module["MyClass"]["getStringFromInstance"](instance); // "hello"
    console.log(temp1);
    instance.delete();
});