
const filter = require('./filter');
let filter_a = new filter();

//var motion_filter_knee = motion_filter.MotionFilter(300); 
var fre_cache = [0,0];
var direction = 0;
var forwarding = false;
var cache_len = 100;
var lastKneeActionMili = 0;
//const map = fre_cache.reduce((m, x) => m.set(x, (m.get(x) || 0) + 1), new Map())

module.exports.processKnee = (data, json, plot=0)=>{
    if(!cof_check(data,27)||!cof_check(data,28)){
        return;
    }
    dis = distance_finder(data,31,32);
    var reads_filter,amp = filter_a.newImpulse(dis,0.15);
    var freq = count(fre_cache)/3.125;

    if (direction >=0){
        if (reads_filter == -1){
            direction = -1;
            lastKneeActionMili = current_milli_time();
            if (fre_cache.length===cache_len){
                fre_cache.splice(0,1);
            }
            fre_cache.push(1);

            json['name'] = 'LKneeUp';
            json['Scope']  = String(amp);
            json['Freq'] = String(freq);
        }
        else{
            if(fre_cache.length === cache_len){
                fre_cache.splice(0,1);
            }
            fre_cache.push(0);
        }
        if(reads_filter === 1){
                direction = 1;
                lastKneeActionMili = current_milli_time();
                if (fre_cache.length === cache_len){
                    fre_cache.splice(0,1);//去掉队列首
                }
                fre_cache.push(1);//
                json['action'] = 'RKneeUp'
                json['Scope'] =  String(amp) 
                json['Freq'] = String(freq);
                if (fre_cache === cache_len){
                    fre_cache.splice(0,1);
                }
                fre_cache.push(0);

            if (current_milli_time()-lastKneeActionMili>500){
                forwarding = false;
            }
            else{
                forwarding = true;
            }
            if (plot){
                motion_filter_knee.debugPlot()
            }
        }
    }
        return json;
}

/* var cof_check = (data,num)=>{
        point = data[num];
        cof = point[4]
        if(cof>0.8){
            return true;
        }
        return false;
    } */
function cof_check(data, num) {
    if(data[num].score > 0.8) return true;
    else return false;
}

var point2vec = (data,num)=>{
    point = data[num];
    vec = [0,0,0];
    vec[0] = point[1];
    vec[1] = point[2];
    vec[2] = point[3];
    return vec;
}

function calVecAngle(data, num1, num2, num3) {
    vec1 = data[num1];
    vec2 = data[num2];
    vec3 = data[num3];
    Lba = Math.sqrt((vec1.x-vec2.x)**2+(vec1.y-vec2.y)**2+(vec1.z-vec2.z)**2);
    Lbc = Math.sqrt((vec3.x-vec2.x)**2+(vec3.y-vec2.y)**2+(vec3.z-vec2.z)**2);
    Labc = (vec1.x-vec2.x)*(vec3.x-vec2.x)+(vec1.y-vec2.y)*(vec3.y-vec2.y)+(vec1.z-vec2.z)*(vec3.z-vec2.z)
    cosAngle = Labc/(Lba*Lbc);
    angle = Math.acos(cosAngle);
    return angle*360/2/Math.PI;
}

var current_milli_time = ()=>{
    var timestamp = Date.parse(new Date());
    return Math.round(timestamp);
}


function distance_finder(data, num1, num2) {
    vec1 = data[num1];
    vec2 = data[num2];
    return (((vec2.x-vec1.x)**2)+((vec2.y-vec1.y)**2)+((vec2.z-vec1.z)**2))**(1/2);
}

var count = (datas)=>{
    var num = 0;
    for(data in datas){
        if(data ===1){
            num++;
        }
    }
    return num;
}