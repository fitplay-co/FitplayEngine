//depth estimator 
//1. calculate depth estimation by compare world and camera 
//2. estimate rough ly body center z toward camera

var depthEstimator = {
    rig_list : [],
    tracing : false,
    current_distance : 0,
    widthScale : 640,
    heightScale : 480,
    startX : 0,
    startY : 0,
    startZ :0,
    pre_x :0,
    pre_z:0,
    pre_y:0,
    centerPointX : 320,
    centerPointY : 240,
    f : 0.05,
    f_dx : 500, //  f/dx
    f_dy : 700,
    legLength : 0,
    whichLeg : 0, //用来识别以左右哪只脚的y为准，0代表左脚，1代表右脚
    appendRig : function(startPoint, endPoint) {
        /*storage in rig_list as 
        [0]start_rig [1]end_rig, [2]current_magnitude, [3]reference_magnitude, [4]reference_distance*/
        this.rig_list.push([startPoint, endPoint, 0, 0, 0])
    },

    process : function(pose, monitor = true) {
        //console.log(pose.keypoints3D)
        if (pose.action ==='reset'){
            this.startX = this.pre_x
            this.startY = this.pre_y
            this.startZ = this.pre_z
            return
        }
        var cameraParam = [[this.f_dx,0,this.centerPointX],
                    [0,this.f_dy,this.centerPointY],
                    [0,0,1]]
        cameraParam = this.inv(cameraParam)
        z_down = this.distance_finder_z_filtered(pose,23,24)*2.6
        var arr_3down = new Array()
        var arr33 = new Array()
        var foot = new Array()
        //左右臀部keypoints坐标
        var data_down = [pose.keypoints[23].x*this.widthScale*z_down,(1-pose.keypoints[23].y)*this.heightScale*z_down,z_down]
        var data = [pose.keypoints[24].x*this.widthScale*z_down,(1-pose.keypoints[24].y)*this.heightScale*z_down,z_down]
        //脚部坐标
        var footLoction = [pose.keypoints3D[31+this.whichLeg].x*this.widthScale*z_down,(1-pose.keypoints[31+this.whichLeg].y)*this.heightScale*z_down,z_down]
        arr_3down = this.invert(data_down,arr_3down)
        arr33 = this.invert(data,arr33)
        foot = this.invert(footLoction,foot)
        //console.log(arr_2)
        //console.log(arr_3)
        //console.log(res)
        var res_down = this.matrixMultiplication(cameraParam,arr_3down)
        var resDown = this.matrixMultiplication(cameraParam,arr33)
        var footRes = this.matrixMultiplication(cameraParam,foot)
        this.legLength = this.distance_finder_leg(this.legLength,pose,z_down)
        res_down[0][0] = (res_down[0][0] + resDown[0][0])/2
        res_down[0][0] = res_down[0][0]*0.1+this.pre_x*0.9
        res_down[1][0] = (res_down[1][0] + resDown[1][0])/2
        //console.log(this.legLength)
        console.log(footRes[1][0])
        console.log(this.legLength)
        res_down[1][0] = this.legLength - Math.abs(footRes[1][0]) 
        res_down[1][0] = res_down[1][0]*0.1+this.pre_y*0.9
        z_down = z_down*0.1+this.pre_z*0.9
        //console.log(pose.action)
        if(this.tracing == false){
            //console.log(pose.action)
            this.startX = res_down[0][0]
            this.startY = res_down[1][0]
            this.startZ = z_down
            this.tracing = true
        }
        if(monitor) {
             pose.monitor = {
                 "rawData_z": (pose.keypoints[23].z+pose.keypoints[24].z)*0.5,
                 "watchData_z" :z_down - this.startZ,
                 "rawData_x":(pose.keypoints[23].x+pose.keypoints[24].x)*0.5,
                 "watchData_x":res_down[0][0] - this.startX,
                 "rawData_y":1-(pose.keypoints[23].y+pose.keypoints[24].y)*0.5,
                 "watchData_y":res_down[1][0] 
             }
         }

        pose.ground_location = {
            //return x with ground location x axis
            x : res_down[0][0]-this.startX,
            y : res_down[1][0]-this.startY,
            z : z_down-this.startZ,
            tracing : this.tracing
        }
        //console.log(pose.ground_location)
        this.pre_x = res_down[0][0]
        this.pre_y = res_down[1][0]
        this.pre_z = z_down
        return 
    },
    distance_finder_z_filtered: function(pose, num1 , num2) {
        vec1 = this.point2vec(pose, num1)
        vec2 = this.point2vec(pose, num2)
        x1 = vec1[0]
        x2 = vec2[0]
        widthScale = this.widthScale
        u1 = this.widthScale * x1
        u2 = this.widthScale * x2
        return 35/Math.abs(u2-u1)
    }, 
    distance_finder_leg : function(legLength,pose,z){
        y_1 = Math.abs(pose.keypoints3D[31].y-pose.keypoints3D[23].y)*this.heightScale*z/(2*18000*this.f)
        //console.log(y_1)
        y_2 = Math.abs(pose.keypoints3D[32].y-pose.keypoints3D[24].y)*this.heightScale*z/(2*18000*this.f)
        //console.log(y_2)
        if (y_1 > legLength){
            legLength = y_1
            this.whichLeg = 0
        }
        if (y_2 > legLength){
            legLength = y_2
            this.whichLeg = 1
        }
        return legLength
        
    },
    det : function(square) {
        if(square.length !== square[0].length) {
            throw new Error();
         }

         // 方阵阶数
         const n = square.length;

         let result = 0;

         if(n > 3) {
            // n 阶
            for(let column = 0; column < n; column++) {
            // 去掉第 0 行第 column 列的矩阵
               const matrix = new Array(n - 1).fill(0).map(arr => new Array(n - 1).fill(0));

               for(let i = 0; i < n - 1; i++) {
                  for(let j = 0; j < n - 1; j++) {
                     if(j < column) {
                        matrix[i][j] = square[i + 1][j];
                     }
                     else {
                        matrix[i][j] = square[i + 1][j + 1];
                     }
                  }
               }

               result += square[0][column] * Math.pow(-1, 0 + column) * det(matrix);
            }
         }
         else if(n === 3) {
            // 3 阶
            result = square[0][0] * square[1][1] * square[2][2] +
                 square[0][1] * square[1][2] * square[2][0] +
                 square[0][2] * square[1][0] * square[2][1] -
                 square[0][2] * square[1][1] * square[2][0] -
                 square[0][1] * square[1][0] * square[2][2] -
                 square[0][0] * square[1][2] * square[2][1];
         }
         else if(n === 2) {
            // 2 阶
            result = square[0][0] * square[1][1] - square[0][1] * square[1][0];
         }
         else if(n === 1) {
            // 1 阶
            result = square[0][0];
         }

         return result;
      },
    
    transpose : function(matrix) {
        var result = new Array(matrix.length).fill(0).map(arr => new Array(matrix[0].length).fill(0));
        for(let i = 0; i < result.length; i++) {
            for(let j = 0; j < result[0].length; j++) {
            result[i][j] = matrix[j][i];
            }
        }
    
        return result;
        },
    adjoint : function(square1) {
        // 方阵约束
        if(square1[0].length !== square1.length) {
            throw new Error();
        }

        let n = square1.length;

        var result = new Array(n).fill(0).map(arr => new Array(n).fill(0));

        for(let row = 0; row < n; row++) {
            for(let column = 0; column < n; column++) {
            // 去掉第 row 行第 column 列的矩阵
                var matrix = [];

                for(let i = 0; i < square1.length; i++) {
                    if(i !== row) {
                    const arr = [];

                    for(let j = 0; j < square1.length; j++) {
                        if(j !== column) {
                            arr.push(square1[i][j]);
                        }
                    }

                    matrix.push(arr);
                    }
                }

                result[row][column] = Math.pow(-1, row + column) * this.det(matrix);
            }
        }

        return this.transpose(result);
        },


    inv : function(square2) {
        if(square2[0].length !== square2.length) {
            throw new Error();
        }
    
        var detValue = this.det(square2);
        var result = this.adjoint(square2);
    
        for(let i = 0; i < result.length; i++) {
            for(let j = 0; j < result.length; j++) {
            result[i][j] /= detValue;
            }
        }
    
        return result;
        },
    invert : function(array2,array22){
        for(var j=0;j<array2.length;j++)
            {
                array22[j] = new Array();
                array22[j][0]=array2[j];
            }        
        return array22;
    },
    matrixMultiplication : function(a,b){
        let len=a.length;
        //console.log(b)
        var len_j = b[0].length;
        let arr=[];
        for(let i=0;i<len;i++){
                arr[i]=[];
                for(var j=0;j<len_j;j++){
                    arr[i][j]=0;//每次都重新置为0
                    for(var k=0;k<len;k++)
                    {
                        arr[i][j]+=a[i][k]*b[k][j];//
                    }
                }
        }
        return arr;
        },

    

    point2vec: function(pose, num) {
        return [pose.keypoints[num].x, pose.keypoints[num].y, pose.keypoints[num].z]
    }
}



module.exports = depthEstimator;