function distanceFinder(data, num1, num2) {
    vec1 = data[num1];
    vec2 = data[num2];
    return (((vec2.x-vec1.x)**2)+((vec2.y-vec1.y)**2)+((vec2.z-vec1.z)**2))**(1/2);
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

function verticalFinder(data, num1, num2) {
    return data[num1].z - data[num2].z;
}

function cofCheck(data, num) {
    if(data[num].score > 0.8) return true;
    else return false;
}

//init json data

// var json = '{"keypoints":[{"x":246.84488117694855,"y":212.465500831604,"z":-0.5676871538162231,"score":0.9997349977493286,"name":"nose"},{"x":253.3892673254013,"y":200.1219606399536,"z":-0.5296849608421326,"score":0.9994720816612244,"name":"left_eye_inner"},{"x":260.24649024009705,"y":200.84524154663086,"z":-0.5301468968391418,"score":0.9994507431983948,"name":"left_eye"},{"x":266.1960697174072,"y":201.49789810180664,"z":-0.5301470160484314,"score":0.9996337890625,"name":"left_eye_outer"},{"x":238.17240357398987,"y":200.78051090240479,"z":-0.5155695080757141,"score":0.9995657801628113,"name":"right_eye_inner"},{"x":233.3500438928604,"y":201.5267515182495,"z":-0.5160356760025024,"score":0.999610960483551,"name":"right_eye"},{"x":228.4822529554367,"y":202.4292755126953,"z":-0.516434371471405,"score":0.9996590614318848,"name":"right_eye_outer"},{"x":272.3840868473053,"y":209.03307437896729,"z":-0.30870872735977173,"score":0.999750018119812,"name":"left_ear"},{"x":222.04553067684174,"y":211.62865161895752,"z":-0.24332405626773834,"score":0.9991813898086548,"name":"right_ear"},{"x":258.1170952320099,"y":227.36951351165771,"z":-0.4808809161186218,"score":0.9998046159744263,"name":"mouth_left"},{"x":238.82651388645172,"y":228.4382200241089,"z":-0.46714717149734497,"score":0.9997342228889465,"name":"mouth_right"},{"x":322.16174483299255,"y":294.1051197052002,"z":-0.24364522099494934,"score":0.999981701374054,"name":"left_shoulder"},{"x":195.08852541446686,"y":306.12908363342285,"z":-0.03900260105729103,"score":0.9998945593833923,"name":"right_shoulder"},{"x":363.90037178993225,"y":393.7449073791504,"z":-0.4348296821117401,"score":0.9669312238693237,"name":"left_elbow"},{"x":180.43127238750458,"y":400.2931594848633,"z":-0.06672428548336029,"score":0.905422031879425,"name":"right_elbow"},{"x":326.08508706092834,"y":391.33832931518555,"z":-0.8449084758758545,"score":0.986647367477417,"name":"left_wrist"},{"x":184.59583640098572,"y":466.5449810028076,"z":-0.4149886667728424,"score":0.9680719375610352,"name":"right_wrist"},{"x":305.97920536994934,"y":393.6023998260498,"z":-0.9477095603942871,"score":0.9860854148864746,"name":"left_pinky"},{"x":189.49764847755432,"y":491.3235855102539,"z":-0.5078094601631165,"score":0.9703283905982971,"name":"right_pinky"},{"x":296.2613260746002,"y":378.5061836242676,"z":-0.9642925262451172,"score":0.9870441555976868,"name":"left_index"},{"x":197.5410407781601,"y":481.31458282470703,"z":-0.5627877712249756,"score":0.9712682962417603,"name":"right_index"},{"x":298.12936663627625,"y":378.3335208892822,"z":-0.855630099773407,"score":0.9812722206115723,"name":"left_thumb"},{"x":199.72655296325684,"y":471.2833499908447,"z":-0.44759368896484375,"score":0.9586179852485657,"name":"right_thumb"},{"x":300.45416593551636,"y":502.31689453125,"z":-0.05626927688717842,"score":0.999949038028717,"name":"left_hip"},{"x":224.4403839111328,"y":496.6139602661133,"z":0.055958595126867294,"score":0.9999149441719055,"name":"right_hip"},{"x":292.88933873176575,"y":649.7480964660645,"z":-0.039617326110601425,"score":0.8871936798095703,"name":"left_knee"},{"x":192.73176491260529,"y":620.9555053710938,"z":-0.2771505117416382,"score":0.8935208916664124,"name":"right_knee"},{"x":293.2481861114502,"y":767.7742195129395,"z":0.4213343560695648,"score":0.8953390121459961,"name":"left_ankle"},{"x":223.78714263439178,"y":676.2796783447266,"z":0.46281886100769043,"score":0.8245459198951721,"name":"right_ankle"},{"x":293.3884871006012,"y":777.8627586364746,"z":0.4600132405757904,"score":0.6171433329582214,"name":"left_heel"},{"x":238.13030362129211,"y":675.1259422302246,"z":0.519175112247467,"score":0.5609354972839355,"name":"right_heel"},{"x":284.08925771713257,"y":842.1597862243652,"z":0.20839110016822815,"score":0.8941788077354431,"name":"left_foot_index"},{"x":212.4103707075119,"y":743.8327789306641,"z":0.39037883281707764,"score":0.823968231678009,"name":"right_foot_index"}],"keypoints3D":[{"x":-0.04928639158606529,"y":-0.6318811178207397,"z":-0.3030053675174713,"score":0.9997349977493286,"name":"nose"},{"x":-0.02491271309554577,"y":-0.6514909863471985,"z":-0.2930477261543274,"score":0.9994720816612244,"name":"left_eye_inner"},{"x":-0.02417352795600891,"y":-0.6521633267402649,"z":-0.28189361095428467,"score":0.9994507431983948,"name":"left_eye"},{"x":-0.02540568634867668,"y":-0.6518268585205078,"z":-0.28468286991119385,"score":0.9996337890625,"name":"left_eye_outer"},{"x":-0.04982660338282585,"y":-0.661857008934021,"z":-0.29771560430526733,"score":0.9995657801628113,"name":"right_eye_inner"},{"x":-0.048682812601327896,"y":-0.6600411534309387,"z":-0.31054389476776123,"score":0.999610960483551,"name":"right_eye"},{"x":-0.043043676763772964,"y":-0.6481108665466309,"z":-0.2911424934864044,"score":0.9996590614318848,"name":"right_eye_outer"},{"x":0.05677426978945732,"y":-0.6370344161987305,"z":-0.19896641373634338,"score":0.999750018119812,"name":"left_ear"},{"x":-0.08192650228738785,"y":-0.5922834873199463,"z":-0.16518297791481018,"score":0.9991813898086548,"name":"right_ear"},{"x":-0.005962054710835218,"y":-0.6197577118873596,"z":-0.2586098909378052,"score":0.9998046159744263,"name":"mouth_left"},{"x":-0.041946712881326675,"y":-0.5868192315101624,"z":-0.27623221278190613,"score":0.9997342228889465,"name":"mouth_right"},{"x":0.16912686824798584,"y":-0.471977174282074,"z":-0.09726355969905853,"score":0.999981701374054,"name":"left_shoulder"},{"x":-0.1396462619304657,"y":-0.5178054571151733,"z":0.0035162142012268305,"score":0.9998945593833923,"name":"right_shoulder"},{"x":0.2548319399356842,"y":-0.2869289219379425,"z":-0.1692880094051361,"score":0.9669312238693237,"name":"left_elbow"},{"x":-0.17103949189186096,"y":-0.2339627593755722,"z":-0.029575703665614128,"score":0.905422031879425,"name":"right_elbow"},{"x":0.16333721578121185,"y":-0.2340826392173767,"z":-0.24670709669589996,"score":0.986647367477417,"name":"left_wrist"},{"x":-0.17769573628902435,"y":-0.10110524296760559,"z":-0.14448584616184235,"score":0.9680719375610352,"name":"right_wrist"},{"x":0.11878127604722977,"y":-0.2246369570493698,"z":-0.258995920419693,"score":0.9860854148864746,"name":"left_pinky"},{"x":-0.16232553124427795,"y":-0.06980868428945541,"z":-0.19500359892845154,"score":0.9703283905982971,"name":"right_pinky"},{"x":0.08100125938653946,"y":-0.2565076947212219,"z":-0.27006420493125916,"score":0.9870441555976868,"name":"left_index"},{"x":-0.12448958307504654,"y":-0.11326640844345093,"z":-0.21358126401901245,"score":0.9712682962417603,"name":"right_index"},{"x":0.14567261934280396,"y":-0.22952812910079956,"z":-0.24934205412864685,"score":0.9812722206115723,"name":"left_thumb"},{"x":-0.1631525754928589,"y":-0.10830912739038467,"z":-0.15919040143489838,"score":0.9586179852485657,"name":"right_thumb"},{"x":0.0941830575466156,"y":0.0017924518324434757,"z":0.01319341454654932,"score":0.999949038028717,"name":"left_hip"},{"x":-0.09175346791744232,"y":-0.0047336481511592865,"z":-0.011239889077842236,"score":0.9999149441719055,"name":"right_hip"},{"x":0.09023427218198776,"y":0.41101908683776855,"z":-0.03328744322061539,"score":0.8871936798095703,"name":"left_knee"},{"x":-0.13273487985134125,"y":0.26898908615112305,"z":-0.11142557114362717,"score":0.8935208916664124,"name":"right_knee"},{"x":0.10792115330696106,"y":0.7109782695770264,"z":0.16646863520145416,"score":0.8953390121459961,"name":"left_ankle"},{"x":-0.0974651575088501,"y":0.49929454922676086,"z":0.22536732256412506,"score":0.8245459198951721,"name":"right_ankle"},{"x":0.08612020313739777,"y":0.7458244562149048,"z":0.19401989877223969,"score":0.6171433329582214,"name":"left_heel"},{"x":-0.09429323673248291,"y":0.5136767029762268,"z":0.22495590150356293,"score":0.5609354972839355,"name":"right_heel"},{"x":0.064444899559021,"y":0.8274310827255249,"z":0.16207517683506012,"score":0.8941788077354431,"name":"left_foot_index"},{"x":-0.10415593534708023,"y":0.5914798378944397,"z":0.20553043484687805,"score":0.823968231678009,"name":"right_foot_index"}],"type":"pose_landmark"}'   
// obj = JSON.parse(json);
// data = obj.keypoints;

// find target point

// var newArr = data.filter(function(p){ 
//     return p.name === "left_eye_inner"; 
//   }); 
// console.log(newArr[0].x);

// test functions

// res1 = distanceFinder(data, 3, 4);
// res2 = cofCheck(data, 21);
// res3 = calVecAngle(data, 5, 6, 7);
// console.log(res3);