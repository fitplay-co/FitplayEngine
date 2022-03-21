walk_processor = require('./action_detection/walk_detection')

var actionEstimator = {
    process : function(pose, monitor = false){
        //不再使用了，明天该成用帧间位移
        //1、累计两脚移动位移，积累速率
        //2、两脚Y轴相对高低来控制跑步左/右方向
        pose.action_detection = {
            "version" : "0.1.0"
        }
        walk_processor.process(pose, monitor)
    }
}

module.exports = actionEstimator;