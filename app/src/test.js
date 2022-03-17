var motion = require('./motion');
let m = new motion();
var res = m.newImpulse(15, 0.05);
console.log(res[0]);