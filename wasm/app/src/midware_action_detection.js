jump_processor = require('./action_detection/jump_detection')
walk_processor = require('./action_detection/walk_detection')

var actionEstimator = {
    process : function(pose, monitor = false){
        //不再使用了，明天该成用帧间位移
        //1、累计两脚移动位移，积累速率
        //2、两脚Y轴相对高低来控制跑步左/右方向
        const resultData = {}
        resultData.action_detection = {
            "version" : "0.1.0"
        }
        walk_processor.process(pose, resultData, monitor)
        jump_processor.process(pose, resultData, monitor)
        return resultData
    }
}

module.exports = actionEstimator;