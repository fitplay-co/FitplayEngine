const {performance} = require('perf_hooks')

var jump_processor = {
    current_distance_mean : 0,
    current_speed_mean: 0,
    current_frame_distance : 0,
    current_jump: 0,
    current_height: 0,
    speed_mean: 0,
    amp_threshold: 0.25,
    monitor_process : false,
    fpm : 0,
    times : [],
    fpmStopCount : 0,
    frameShiftFilterCount : 0,
    resultData: {},

    process : function(pose, resultData, monitor = false){
        this.resultData = resultData
        this.calculate_current_frame_distance(pose)
        this.calculate_current_distance_mean(pose)
        this.calculate_current_direction(pose, monitor)
        this.monitor_process = monitor
        return this.resultData
    }, 

    /*1. calculate current frame point weighted summation
     */
    calculate_current_frame_distance: function(pose) {
        this.current_frame_distance = 0
        this.current_frame_distance = this.weighted_sum(pose)
    },

    calculate_current_distance_mean: function(pose) {
        this.current_distance_mean = this.current_distance_mean * 0.8 + this.current_frame_distance * 0.2
    },

    calculate_current_direction: function(pose, monitor = false) {
        this.current_speed_mean = this.current_speed_mean * 0.9  + Math.abs(this.current_frame_distance - this.current_distance_mean) * 0.1
        if(this.current_frame_distance - this.current_distance_mean > this.amp_threshold) {
            if(this.current_jump != 0) {
                if(this.frameShiftFilterCount > 2) {
                    this.shiftDirectionStepPerMinutes()
                    this.current_jump = 0 
                    this.frameShiftFilterCount = 0
                } else {
                    this.frameShiftFilterCount = this.frameShiftFilterCount + 1
                }
            } else {
                this.frameShiftFilterCount = 0
            }        
        } 
        if(this.current_frame_distance - this.current_distance_mean < -this.amp_threshold) {
            if(this.current_jump != 1) {
                if(this.frameShiftFilterCount > 2) {
                    this.shiftDirectionStepPerMinutes()
                    this.current_jump = 1 
                    this.frameShiftFilterCount = 0
                } else {
                    this.frameShiftFilterCount = this.frameShiftFilterCount + 1
                }
            } else {
                this.frameShiftFilterCount = 0
            }
        } 
        if(Math.abs(this.current_frame_distance - this.current_distance_mean) < this.amp_threshold) {
            this.fpmStopCount = this.fpmStopCount + 1 
            if(this.fpmStopCount > 30) {
                this.current_jump = 0
                this.fpm = 0
            }
        } else {
            this.fpmStopCount = 0
        }
        // console.log(this.current_jump)
        // console.log(this.current_speed_mean)
        this.current_height = this.current_jump == 1? this.current_speed_mean:0
        this.resultData.action_detection.jump = {
            "up" : this.current_jump,
            "strength" : this.current_height
        }
        if (monitor) {
            console.log("freq : " + this.fpm)
            console.log("strength : " + this.current_speed_mean)
            this.resultData.monitor = {
                "rawData": this.current_jump,
                "watchData" :this.current_frame_distance
            }
        }
    },

    shiftDirectionStepPerMinutes : function() { 
        const now = performance.now();
        while (this.times.length > 0 && this.times[0] <= now - 5000) {
            this.times.shift();
        }
        this.times.push(now);
        this.fpm = this.times.length * 20;
    },

    /**2. using a filter to determin summation mean in each frame  */

    weighted_sum: function(pose) {
        res = pose.keypoints[23].y + pose.keypoints[24].y + pose.keypoints[25].y + pose.keypoints[26].y + pose.keypoints[27].y + pose.keypoints[28].y
        return res
    }
}

module.exports = jump_processor