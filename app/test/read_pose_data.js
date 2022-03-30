var fs = require('fs');
var path = require('path'); 
var file = path.join(__dirname, 'jump1.json');
function writeJson(update){
    fs.readFile(file,function(err,data){
        if(err){
            return console.error(err);
        }
        var jump = data.toString();
        jump = JSON.parse(jump);
        jump.ysum.push(update);
        var str = JSON.stringify(jump);
        fs.writeFile(file,str,function(err){
            if(err){
                console.error(err);
            }
            console.log('complete');
        })
    })
}
var readPose = {
    process : function(pose){
        res = pose.keypoints[23].y + pose.keypoints[24].y + pose.keypoints[25].y + pose.keypoints[26].y + pose.keypoints[27].y + pose.keypoints[28].y;
        writeJson(res);
    }
}

module.exports = readPose;

