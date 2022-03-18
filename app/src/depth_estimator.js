//depth estimator 
//1. calculate depth estimation by compare world and camera 
//2. estimate rough ly body center z toward camera
var depthEstimator = {
    rig_list : [],
    tracing : false,
    current_distance : 0,
    withScale : 480,
    heightScale : 640,

    appendRig : function(startPoint, endPoint) {
        /*storage in rig_list as 
        [0]start_rig [1]end_rig, [2]current_magnitude, [3]reference_magnitude, [4]reference_distance*/
        this.rig_list.push([startPoint, endPoint, 0, 0, 0])
    },

    estimateDepth : function(pose){
        return this.estimateDepthUpdate(pose)
    },

    estimateDepthUpdate : function(pose){
        this.calculateCurrentMagnitudeData(pose)
        this.current_distance = this.determineCurrentDistance()
        this.feedbackReferenceDistance(this.current_distance)
        return 1.0 / this.current_distance
    },

    /* 1. update data and calculate sparse current magnitude, will be zero if currently this joint
      has some Z reading grater than 0.01, because z magnitude is not accrate, should be ignore
     */
    calculateCurrentMagnitudeData : function(pose) {
        index = 0
        this.rig_list.forEach(item => {
            item[2] = this.distance_finder_y_filtered(pose, item[0], item[1])
            index = index + 1
            if(item[2] > 0 ) {
                print("filter" + str(index)) 
            }
        });
    }, 

    /* 2. determine current distance base on motion information 
       start tracking when any one item reads a magnitude pose  */   
    determineCurrentDistance : function() {
        if(!this.tracing) {
            this.rig_list.forEach(item => {
                if (item[2] != 0) {
                    this.tracing = true
                    this.current_distance = 1
                    return this.current_distance
                }
            })
        } else {
            var distanceItemCount = 0 
            var distanceSum = 0
            this.rig_list.forEach(item => {
                if(item[2] > 0 && item[3] > 0 && item[4] > 0) {
                    distanceItemCount = distanceItemCount + 1
                    distanceSum = distanceSum + (item[2] * item[4]/ item[3])
                }
            })
            if (distanceItemCount > 0) {
                return distanceSum/distanceItemCount
            } else {
                return this.current_distance
            }
        }
    },

    /* 3. update sparse reference to feedback distance, might be zero if tracing in not initialized
      set up tracing when reference distance is not zero, update them with new chance */
    feedbackReferenceDistance : function(reference_distance) {
        if(this.tracing) {
            this.rig_list.forEach(item => {
                if (item[2] != 0 && item[3]==0 && item[4]==0) {
                    item[3] = item[2]
                    item[4] = reference_distance
                }
            })
        }
    },

    distance_finder_y_filtered: function(pose, num1 , num2) {
        vec1 = this.point2vec(pose, num1)
        vec2 = this.point2vec(pose, num2)
        x1 = vec1[0]
        y1 = vec1[1]
        z1 = vec1[2]
        x2 = vec2[0]
        y2 = vec2[1]
        z2 = vec2[2]
        widthScale = this.widthScale
        heightScale = this.heightScale
        if (Math.abs(z1 - z2) > 0.05) {
            return 0
        } else {
            return Math.pow((x1-x2) * (x1-x2) * widthScale * widthScale + (y1-y2) * (y1-y2) * heightScale * heightScale, 0.5) 
        }
    }, 

    point2vec: function(pose, num) {
        return [pose.keypoints[num].x, pose.keypoints[num].y, pose.keypoints[num].z]
    }
}

depthEstimator.appendRig(11, 12)
depthEstimator.appendRig(23, 24)
depthEstimator.appendRig(12, 24)
depthEstimator.appendRig(11, 23)
depthEstimator.appendRig(8, 7)

module.exports = depthEstimator;