var Module = require('./dist/sample.js');

// var instance = new Module.MyClass(10, "hello");
// instance.incrementX();
// instance.x; // 11
// instance.x = 20; // 20
// var temp = Module.MyClass.getStringFromInstance(instance); // "hello"
// console.log(temp);
// instance.delete();
Module.addOnPostRun(function() { 
    var instance = new Module["MyClass"](10, "hello");
    instance["incrementX"]();
    instance["x"]; // 11
    instance["x"] = 20; // 20
    var temp = Module["MyClass"]["getStringFromInstance"](instance); // "hello"
    console.log(temp);
    instance.delete();
});
