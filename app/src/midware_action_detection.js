var walkProcessor = require("./walkprocesser")

var actionEstimator = {
    process : function(pose){
        //不再使用了，明天该成用帧间位移
        //1、累计两脚移动位移，积累速率
        //2、两脚Y轴相对高低来控制跑步左/右方向
        walkProcessor.processKnee(pose, {})
    }
}

module.exports = actionEstimator;