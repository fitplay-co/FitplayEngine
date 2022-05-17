var walk_processor = {
    rig_list : [],
    current_distance_mean : 0,
    current_speed_mean: 0,
    current_frame_distance : 0,
    current_leg: 0,
    speed_mean: 0,
    amp_threshold: 0.05,
    monitor_process : false,
    fpm : 0,
    times : [],
    fpmStopCount : 0,
    frameShiftFilterCount : 0 ,

    process : function(pose, monitor = true){
        this.calculate_current_frame_distance(pose)
        this.calculate_current_distance_mean(pose)
        this.calculate_current_direction(pose, monitor)
        
        this.monitor_process = monitor
    }, 

    appendRig : function(startPoint, endPoint) {
        /*storage in rig_list as 
        [0]start_rig [1]end_rig */
        this.rig_list.push([startPoint, endPoint])
    },

    /*1. calculate current drame point distance in each leg 
     */
    calculate_current_frame_distance: function(pose) {
        this.current_frame_distance = 0
        this.rig_list.forEach(item => {
            this.current_frame_distance = this.current_frame_distance
             + this.distance_finder_y_axis(pose, item[0], item[1])
        });
    },

    calculate_current_distance_mean: function(pose) {
        this.current_distance_mean = this.current_distance_mean * 0.9 + this.current_frame_distance * 0.1
    },

    calculate_current_direction: function(pose, monitor = true) {
        this.current_speed_mean = this.current_speed_mean * 0.9  + Math.abs(this.current_frame_distance - this.current_distance_mean) * 0.1
        if(this.current_frame_distance  > this.amp_threshold) {
            if(this.current_leg != 1) {
                if(this.frameShiftFilterCount > 2) {
                    this.shiftDirectionStepPerMinutes()
                    this.current_leg = 1 
                    this.frameShiftFilterCount = 0
                } else {
                    this.frameShiftFilterCount = this.frameShiftFilterCount + 1
                }
            } else {
                this.frameShiftFilterCount = 0
            }        
        } 
        if(this.current_frame_distance  < -this.amp_threshold) {
            if(this.current_leg != -1) {
                if(this.frameShiftFilterCount > 2) {
                    this.shiftDirectionStepPerMinutes()
                    this.current_leg = -1 
                    this.frameShiftFilterCount = 0
                } else {
                    this.frameShiftFilterCount = this.frameShiftFilterCount + 1
                }
            } else {
                this.frameShiftFilterCount = 0
            }
        } 
        if(Math.abs(this.current_frame_distance ) < this.amp_threshold) {
            this.fpmStopCount = this.fpmStopCount + 1 
            if(this.fpmStopCount > 15) {
                this.current_leg = 0
                this.fpm = 0
            }
        } else {
            this.fpmStopCount = 0
        }
        pose.action_detection.walk = {
            "legUp" : this.current_leg,
            "frequency" : this.fpm,
            "strength" : this.current_speed_mean
        }
        //console.log(pose.action_detection.walk.legUp)
        //console.log(pose.action_detection.walk.frequency)
        if (monitor) {
            //console.log("freq : " + this.fpm)
            //console.log("strength : " + this.current_speed_mean)
            pose.monitor = {
                "rawData_x": this.current_leg,
                "watchData_x" :this.current_frame_distance,
                "rawData_y" : this.fpm,
                "watchData_y" : this.fpm

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

    /**2. using a filter to determin distance mean in each frame  */

    distance_finder_y_axis: function(pose, num1 , num2) {
        vec1 = this.point2vec(pose, num1)
        vec2 = this.point2vec(pose, num2)
        x1 = vec1[0]
        y1 = vec1[1]
        z1 = vec1[2]
        x2 = vec2[0]
        y2 = vec2[1]
        z2 = vec2[2]
        return y1-y2
    }, 

    point2vec: function(pose, num) {
        return [pose.keypoints[num].x, pose.keypoints[num].y, pose.keypoints[num].z]
    }
}


walk_processor.appendRig(26, 25)
walk_processor.appendRig(28, 27)
walk_processor.appendRig(32, 31)

module.exports = walk_processor