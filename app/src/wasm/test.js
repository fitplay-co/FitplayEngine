var flatbuffers = require('./flatbuffers/flatbuffers.js');
var Point = require('./flatbuffers/point').Point
var Pose = require('./flatbuffers/pose.js').Pose;
var fs = require('fs')


var test = {
    process : function(){
        var bytes = new Uint8Array(fs.readFileSync('./123.bin'));

        var buf = new flatbuffers.ByteBuffer(bytes);
        var pose = Pose.getRootAsPose(buf)
        console.dir(pose.bb.bytes_,{'maxArrayLength': null})
        var keypoint = pose.keypoints(1)
        
        
        // console.log(typeof(keypoint))
        // var buff = new flatbuffers.ByteBuffer(keypoint)
        // var point = Point.getRootAsPoint(buff)

        console.log(keypoint)
    }
}
module.exports = test