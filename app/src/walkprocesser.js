
const filter = require('./filter');
let filter_a = new filter();

//var motion_filter_knee = motion_filter.MotionFilter(300); 
var fre_cache = [0,0];
var direction = 0;
var forwarding = false;
var cache_len = 100;
var lastKneeActionMili = 0;
//const map = fre_cache.reduce((m, x) => m.set(x, (m.get(x) || 0) + 1), new Map())

module.exports.processKnee = (data,json,plot=0)=>{
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
            json['Scope']  =String(amp);
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

/* var calVecAngle = (data,num1,num2,num3)=>{
    vec1 = point2vec(data,num1);
    vec2 = point2vec(data,num2);
    vec3 = point2vec(data,num3);
    lba = Math.sqrt(matrixMul((vec1-vec2),(vec1-vec2)));
    lbc = Math.sqrt(matrixMul((vec3-vec2),(vec3-vec2)));
    cos_angle = matrixMul((vec1-vec2),(vec3-vec2))/(lba*lbc);
    for(angle in cos_angle){
        angle = Math.acos(angle);
    }
    angle2 = cos_angle*360/2/Math.PI;
    return angle2;
} */
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

/* var matrixMul = (a,b)=>{
	var c=new Array(a.length);
	for(var i=0;i<a.length;i++)
	{
		c[i]=new Array(b[0].length);
		for(var j=0;j<b[0].length;j++)
		{
			c[i][j]=0;
			for(var k=0;k<b.length;k++)
			{
				c[i][j]+=a[i][k]*b[k][j];
			}
		}
	}
	return c;
} */

var current_milli_time = ()=>{
    var timestamp = Date.parse(new Date());
    return Math.round(timestamp);
}

/* var distance_finder = (data,num1,num2)=>{
    vec1 = point2vec(data,num1);
    vec2 = point2vec(data,num2);
    return vec1[2]-vec2[2];
} */
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
//var json = {};
//data = [['']['0', '0.49790090322494507', '0.3049525022506714', '-0.19463415443897247', '0.9999985694885254'],['1', '0.5144931674003601', '0.2935986816883087', '-0.16964516043663025', '0.999994158744812'],
/* ['2', '0.5251189470291138', '0.2940697968006134', '-0.16967818140983582', '0.9999939203262329'],['3', '0.5328274965286255', '0.29442575573921204', '-0.16973698139190674', '0.999994158744812'],
['4', '0.4837700426578522', '0.2931493818759918', '-0.16625380516052246', '0.9999938011169434'],['5', '0.4740484356880188', '0.2933575212955475', '-0.16607490181922913', '0.999992847442627'],
['6', '0.46502262353897095', '0.2936893701553345', '-0.16591140627861023', '0.9999926090240479'],['7', '0.5452199578285217', '0.30180028080940247', '-0.011482861824333668', '0.9999903440475464'],
['8', '0.45820504426956177', '0.3012144863605499', '0.002535973209887743', '0.9999877214431763'],['9', '0.5177396535873413', '0.3221287727355957', '-0.13240033388137817', '0.9999971389770508'],
['10', '0.4803094267845154', '0.32158902287483215', '-0.1280774027109146', '0.9999960660934448'],['11', '0.6148916482925415', '0.38854163885116577', '0.04096626862883568', '0.9999957084655762'],
['12', '0.39392799139022827', '0.38883712887763977', '0.06132503226399422', '0.9999853372573853'],['13', '0.6521264314651489', '0.4832839369773865', '0.12175805121660233', '0.9871893525123596'],
['14', '0.36550623178482056', '0.488080769777298', '0.11090429127216339', '0.9689375758171082'],['15', '0.6628208160400391', '0.5664533972740173', '0.042441800236701965', '0.9912755489349365'],
['16', '0.33643192052841187', '0.575545608997345', '-0.015868468210101128', '0.9694990515708923'],['17', '0.6736617684364319', '0.5945238471031189', '0.0018091771053150296', '0.9841011166572571'],
['18', '0.3213499188423157', '0.6051022410392761', '-0.0679030492901802', '0.9542617797851562'],['19', '0.6574148535728455', '0.5935497283935547', '-0.05758606642484665', '0.9846023321151733'],
['20', '0.33551687002182007', '0.6050958037376404', '-0.13225287199020386', '0.9572822451591492'],['21', '0.6492737531661987', '0.5840494632720947', '0.018738213926553726', '0.9822970032691956'],
['22', '0.3445966839790344', '0.5960853099822998', '-0.047649119049310684', '0.9560409188270569'],['23', '0.5743703246116638', '0.5841991901397705', '-0.006203243043273687', '0.9999239444732666'],
['24', '0.4435080885887146', '0.585938036441803', '0.0059629809111356735', '0.999913215637207'],['25', '0.5702012777328491', '0.7472540140151978', '-0.03489953652024269', '0.9936789870262146'],
['26', '0.45291566848754883', '0.7457014918327332', '6.604258669540286e-05', '0.989715039730072'],['27', '0.5629662871360779', '0.8768270015716553', '0.23069855570793152', '0.9843403100967407'],
['28', '0.47017237544059753', '0.877751350402832', '0.25515469908714294', '0.9840559959411621'],['29', '0.548084557056427', '0.8938120603561401', '0.2446228414773941', '0.7922069430351257'],
['30', '0.48547637462615967', '0.89534592628479', '0.26810646057128906', '0.8121219277381897'],['31', '0.5878176093101501', '0.9308444261550903', '0.0402991808950901', '0.9703676700592041'],
['32', '0.4330928325653076', '0.9313523769378662', '0.06890103220939636', '0.9728092551231384'],['']] */
//var json = '{"keypoints":[{"x":246.84488117694855,"y":212.465500831604,"z":-0.5676871538162231,"score":0.9997349977493286,"name":"nose"},{"x":253.3892673254013,"y":200.1219606399536,"z":-0.5296849608421326,"score":0.9994720816612244,"name":"left_eye_inner"},{"x":260.24649024009705,"y":200.84524154663086,"z":-0.5301468968391418,"score":0.9994507431983948,"name":"left_eye"},{"x":266.1960697174072,"y":201.49789810180664,"z":-0.5301470160484314,"score":0.9996337890625,"name":"left_eye_outer"},{"x":238.17240357398987,"y":200.78051090240479,"z":-0.5155695080757141,"score":0.9995657801628113,"name":"right_eye_inner"},{"x":233.3500438928604,"y":201.5267515182495,"z":-0.5160356760025024,"score":0.999610960483551,"name":"right_eye"},{"x":228.4822529554367,"y":202.4292755126953,"z":-0.516434371471405,"score":0.9996590614318848,"name":"right_eye_outer"},{"x":272.3840868473053,"y":209.03307437896729,"z":-0.30870872735977173,"score":0.999750018119812,"name":"left_ear"},{"x":222.04553067684174,"y":211.62865161895752,"z":-0.24332405626773834,"score":0.9991813898086548,"name":"right_ear"},{"x":258.1170952320099,"y":227.36951351165771,"z":-0.4808809161186218,"score":0.9998046159744263,"name":"mouth_left"},{"x":238.82651388645172,"y":228.4382200241089,"z":-0.46714717149734497,"score":0.9997342228889465,"name":"mouth_right"},{"x":322.16174483299255,"y":294.1051197052002,"z":-0.24364522099494934,"score":0.999981701374054,"name":"left_shoulder"},{"x":195.08852541446686,"y":306.12908363342285,"z":-0.03900260105729103,"score":0.9998945593833923,"name":"right_shoulder"},{"x":363.90037178993225,"y":393.7449073791504,"z":-0.4348296821117401,"score":0.9669312238693237,"name":"left_elbow"},{"x":180.43127238750458,"y":400.2931594848633,"z":-0.06672428548336029,"score":0.905422031879425,"name":"right_elbow"},{"x":326.08508706092834,"y":391.33832931518555,"z":-0.8449084758758545,"score":0.986647367477417,"name":"left_wrist"},{"x":184.59583640098572,"y":466.5449810028076,"z":-0.4149886667728424,"score":0.9680719375610352,"name":"right_wrist"},{"x":305.97920536994934,"y":393.6023998260498,"z":-0.9477095603942871,"score":0.9860854148864746,"name":"left_pinky"},{"x":189.49764847755432,"y":491.3235855102539,"z":-0.5078094601631165,"score":0.9703283905982971,"name":"right_pinky"},{"x":296.2613260746002,"y":378.5061836242676,"z":-0.9642925262451172,"score":0.9870441555976868,"name":"left_index"},{"x":197.5410407781601,"y":481.31458282470703,"z":-0.5627877712249756,"score":0.9712682962417603,"name":"right_index"},{"x":298.12936663627625,"y":378.3335208892822,"z":-0.855630099773407,"score":0.9812722206115723,"name":"left_thumb"},{"x":199.72655296325684,"y":471.2833499908447,"z":-0.44759368896484375,"score":0.9586179852485657,"name":"right_thumb"},{"x":300.45416593551636,"y":502.31689453125,"z":-0.05626927688717842,"score":0.999949038028717,"name":"left_hip"},{"x":224.4403839111328,"y":496.6139602661133,"z":0.055958595126867294,"score":0.9999149441719055,"name":"right_hip"},{"x":292.88933873176575,"y":649.7480964660645,"z":-0.039617326110601425,"score":0.8871936798095703,"name":"left_knee"},{"x":192.73176491260529,"y":620.9555053710938,"z":-0.2771505117416382,"score":0.8935208916664124,"name":"right_knee"},{"x":293.2481861114502,"y":767.7742195129395,"z":0.4213343560695648,"score":0.8953390121459961,"name":"left_ankle"},{"x":223.78714263439178,"y":676.2796783447266,"z":0.46281886100769043,"score":0.8245459198951721,"name":"right_ankle"},{"x":293.3884871006012,"y":777.8627586364746,"z":0.4600132405757904,"score":0.6171433329582214,"name":"left_heel"},{"x":238.13030362129211,"y":675.1259422302246,"z":0.519175112247467,"score":0.5609354972839355,"name":"right_heel"},{"x":284.08925771713257,"y":842.1597862243652,"z":0.20839110016822815,"score":0.8941788077354431,"name":"left_foot_index"},{"x":212.4103707075119,"y":743.8327789306641,"z":0.39037883281707764,"score":0.823968231678009,"name":"right_foot_index"}],"keypoints3D":[{"x":-0.04928639158606529,"y":-0.6318811178207397,"z":-0.3030053675174713,"score":0.9997349977493286,"name":"nose"},{"x":-0.02491271309554577,"y":-0.6514909863471985,"z":-0.2930477261543274,"score":0.9994720816612244,"name":"left_eye_inner"},{"x":-0.02417352795600891,"y":-0.6521633267402649,"z":-0.28189361095428467,"score":0.9994507431983948,"name":"left_eye"},{"x":-0.02540568634867668,"y":-0.6518268585205078,"z":-0.28468286991119385,"score":0.9996337890625,"name":"left_eye_outer"},{"x":-0.04982660338282585,"y":-0.661857008934021,"z":-0.29771560430526733,"score":0.9995657801628113,"name":"right_eye_inner"},{"x":-0.048682812601327896,"y":-0.6600411534309387,"z":-0.31054389476776123,"score":0.999610960483551,"name":"right_eye"},{"x":-0.043043676763772964,"y":-0.6481108665466309,"z":-0.2911424934864044,"score":0.9996590614318848,"name":"right_eye_outer"},{"x":0.05677426978945732,"y":-0.6370344161987305,"z":-0.19896641373634338,"score":0.999750018119812,"name":"left_ear"},{"x":-0.08192650228738785,"y":-0.5922834873199463,"z":-0.16518297791481018,"score":0.9991813898086548,"name":"right_ear"},{"x":-0.005962054710835218,"y":-0.6197577118873596,"z":-0.2586098909378052,"score":0.9998046159744263,"name":"mouth_left"},{"x":-0.041946712881326675,"y":-0.5868192315101624,"z":-0.27623221278190613,"score":0.9997342228889465,"name":"mouth_right"},{"x":0.16912686824798584,"y":-0.471977174282074,"z":-0.09726355969905853,"score":0.999981701374054,"name":"left_shoulder"},{"x":-0.1396462619304657,"y":-0.5178054571151733,"z":0.0035162142012268305,"score":0.9998945593833923,"name":"right_shoulder"},{"x":0.2548319399356842,"y":-0.2869289219379425,"z":-0.1692880094051361,"score":0.9669312238693237,"name":"left_elbow"},{"x":-0.17103949189186096,"y":-0.2339627593755722,"z":-0.029575703665614128,"score":0.905422031879425,"name":"right_elbow"},{"x":0.16333721578121185,"y":-0.2340826392173767,"z":-0.24670709669589996,"score":0.986647367477417,"name":"left_wrist"},{"x":-0.17769573628902435,"y":-0.10110524296760559,"z":-0.14448584616184235,"score":0.9680719375610352,"name":"right_wrist"},{"x":0.11878127604722977,"y":-0.2246369570493698,"z":-0.258995920419693,"score":0.9860854148864746,"name":"left_pinky"},{"x":-0.16232553124427795,"y":-0.06980868428945541,"z":-0.19500359892845154,"score":0.9703283905982971,"name":"right_pinky"},{"x":0.08100125938653946,"y":-0.2565076947212219,"z":-0.27006420493125916,"score":0.9870441555976868,"name":"left_index"},{"x":-0.12448958307504654,"y":-0.11326640844345093,"z":-0.21358126401901245,"score":0.9712682962417603,"name":"right_index"},{"x":0.14567261934280396,"y":-0.22952812910079956,"z":-0.24934205412864685,"score":0.9812722206115723,"name":"left_thumb"},{"x":-0.1631525754928589,"y":-0.10830912739038467,"z":-0.15919040143489838,"score":0.9586179852485657,"name":"right_thumb"},{"x":0.0941830575466156,"y":0.0017924518324434757,"z":0.01319341454654932,"score":0.999949038028717,"name":"left_hip"},{"x":-0.09175346791744232,"y":-0.0047336481511592865,"z":-0.011239889077842236,"score":0.9999149441719055,"name":"right_hip"},{"x":0.09023427218198776,"y":0.41101908683776855,"z":-0.03328744322061539,"score":0.8871936798095703,"name":"left_knee"},{"x":-0.13273487985134125,"y":0.26898908615112305,"z":-0.11142557114362717,"score":0.8935208916664124,"name":"right_knee"},{"x":0.10792115330696106,"y":0.7109782695770264,"z":0.16646863520145416,"score":0.8953390121459961,"name":"left_ankle"},{"x":-0.0974651575088501,"y":0.49929454922676086,"z":0.22536732256412506,"score":0.8245459198951721,"name":"right_ankle"},{"x":0.08612020313739777,"y":0.7458244562149048,"z":0.19401989877223969,"score":0.6171433329582214,"name":"left_heel"},{"x":-0.09429323673248291,"y":0.5136767029762268,"z":0.22495590150356293,"score":0.5609354972839355,"name":"right_heel"},{"x":0.064444899559021,"y":0.8274310827255249,"z":0.16207517683506012,"score":0.8941788077354431,"name":"left_foot_index"},{"x":-0.10415593534708023,"y":0.5914798378944397,"z":0.20553043484687805,"score":0.823968231678009,"name":"right_foot_index"}],"type":"pose_landmark"}'   
//obj = JSON.parse(json);
//data = obj.keypoints;

