var gaze_tracking = {
    rig_list : [],
    z : 0,
    widthScale : 640,
    heightScale : 480,
    pre_x :0,
    pre_y:0,
    pre_z:0,
    
    appendRig : function(startPoint, endPoint) {
        /*storage in rig_list as 
        [0]start_rig [1]end_rig, [2]current_magnitude, [3]reference_magnitude, [4]reference_distance*/
        this.rig_list.push([startPoint, endPoint, 0, 0, 0])
    },
    process : function(pose,monitor = true){
        var arr_1 = [[10000,0,320],
             [0,10000,240],
             [0,0,1]]
        var arr_2 = [[0.05,0,0],
            [0,0.05,0],
            [0,0,1]]
        z = this.distance_finder_z_filtered(pose,2,5)
        var arr_3 = new Array()
        var data = [pose.keypoints[0].x*this.widthScale*z,pose.keypoints[0].y*this.heightScale*z,z]
        arr_3 = this.invert(data,arr_3);
        arr_1 = this.inv(arr_1);
        //console.log(arr_1)
        
        arr_2 = this.inv(arr_2);
        //console.log(arr_2)
        //console.log(arr_3)
        var res = this.matrixMultiplication(arr_2,arr_1)
        //console.log(res)
        res = this.matrixMultiplication(res,arr_3);
        res[0][0] = res[0][0]*0.1+this.pre_x*0.9
        res[1][0] = res[1][0]*0.1+this.pre_y*0.9
        z = z*0.1+this.pre_z*0.9
        //console.log(res)
        if(monitor) {
            pose.monitor = {
                "rawData_z": pose.keypoints[0].z,
                "watchData_z" :z,
                "rawData_x":pose.keypoints[0].x,
                "watchData_x":res[0][0],
                "rawData_y":pose.keypoints[0].y,
                "watchData_y":res[1][0]
            }
        }
        pose.gaze_tracking = {
            x: res[0][0],
            y: res[1][0],
            z: z
        }
        this.pre_x = res[0][0]
        //console.log(this.pre_x)
        this.pre_y = res[1][0]
        this.pre_z = z
        return z
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

gaze_tracking.appendRig(2,5)
module.exports = gaze_tracking;