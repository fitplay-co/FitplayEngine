/* ------------------------------------------------ *
 * The MIT License (MIT)
 * Copyright (c) 2020 terryky1220@gmail.com
 * ------------------------------------------------ */
//tf.setBackend('wasm').then(() => startWebGL());

let s_debug_log;
let s_rtarget_main;
let s_rtarget_feed;


/*
 *  pose3d-space coordinate
 *
 *    -100  0  100
 *      +---+---+  100
 *      |   |   |
 *      +---+---+   0
 *      |   |   |
 *      +---+---+ -100
 */
class GuiProperty {
    constructor() {
        this.pose_scale_x = 100;
        this.pose_scale_y = 100;
        this.pose_scale_z = 100;
        this.camera_pos_z = 300;
        this.joint_radius = 8;
        this.bone_radius  = 2;
        this.srcimg_scale = 1.0;
        this.draw_axis   = false;
        this.draw_pmeter = false;
    }
}
const s_gui_prop = new GuiProperty();


let s_srctex_region = {
    width: 0, height: 0,                    /* full rect with margin */
    tex_x: 0, tex_y: 0, tex_w: 0, tex_h: 0  /* valid texture area */
};

function init_stats ()
{
    var stats = new Stats();
    var xPanel = stats.addPanel( new Stats.Panel( 'x', '#ff8', '#221' ) );
    var yPanel = stats.addPanel( new Stats.Panel( 'y', '#f8f', '#212' ) );
    stats.showPanel( 0 );
    document.body.appendChild( stats.dom );

    return stats;
}


/* Adjust the texture size to fit the window size
 *
 *                      Portrait
 *     Landscape        +------+
 *     +-+------+-+     +------+
 *     | |      | |     |      |
 *     | |      | |     |      |
 *     +-+------+-+     +------+
 *                      +------+
 */
function
generate_input_image (gl, texid, src_w, src_h, win_w, win_h)
{
    let dims = get_pose3d_input_dims ();
    let buf_rgba = new Uint8Array (dims.w * dims.h * 4);
    let buf_rgb  = new Uint8Array (dims.w * dims.h * 3);

    let dst_aspect = dims.w / dims.h;
    let tex_aspect = src_w / src_h;
    let scale;
    let scaled_w, scaled_h;
    let offset_x, offset_y;

    if (dst_aspect > tex_aspect)
    {
        scale = dims.h / src_h;
        scaled_w = scale * src_w;
        scaled_h = scale * src_h;
        offset_x = (dims.w - scaled_w) * 0.5;
        offset_y = 0;
    }
    else
    {
        scale = dims.w / src_w;
        scaled_w = scale * src_w;
        scaled_h = scale * src_h;
        offset_x = 0;
        offset_y = (dims.h - scaled_h) * 0.5;
    }

    GLUtil.set_render_target (gl, s_rtarget_feed);
    gl.clear (gl.COLOR_BUFFER_BIT);

    /* draw valid texture area */
    const dx = offset_x;
    const dy = win_h - dims.h + offset_y;
    r2d.draw_2d_texture (gl, texid, dx, dy, scaled_w, scaled_h, 1);

    /* read full rect with margin */
    gl.readPixels (0, 0, dims.w, dims.h, gl.RGBA, gl.UNSIGNED_BYTE, buf_rgba);
    for (let i = 0, j = 0; i < buf_rgba.length; i ++)
    {
        if (i % 4 != 3)
            buf_rgb[j++] = buf_rgba[i];
    }

    GLUtil.set_render_target (gl, s_rtarget_main);

    s_srctex_region.width  = dims.w;    /* full rect width  with margin */
    s_srctex_region.height = dims.h;    /* full rect height with margin */
    s_srctex_region.tex_x  = offset_x;  /* start position of valid texture */
    s_srctex_region.tex_y  = offset_y;  /* start position of valid texture */
    s_srctex_region.tex_w  = scaled_w;  /* width  of valid texture */
    s_srctex_region.tex_h  = scaled_h;  /* height of valid texture */

    return buf_rgb;
}


function
compute_3d_skelton_pos (dst_pose, src_pose)
{
    /*
     *  because key3d[kNeck] is always zero,
     *  we need to add offsets (key2d[kNeck]) to translate it to the global world. 
     */
    const kNeck = 1;
    const neck_x  = src_pose.keypoints[kNeck].x;
    const neck_y  = src_pose.keypoints[kNeck].y;
    const xoffset = (neck_x - 0.5);
    const yoffset = (neck_y - 0.5);

    for (let i = 0; i < kPoseKeyNum; i ++)
    {
        let x = src_pose.keypoints3D[i].x;
        let y = src_pose.keypoints3D[i].y;
        let z = src_pose.keypoints3D[i].z;
        let s = src_pose.keypoints3D[i].score;

        x = (x + xoffset) * s_gui_prop.pose_scale_x * 2;
        y = (y + yoffset) * s_gui_prop.pose_scale_y * 2;
        z = z * s_gui_prop.pose_scale_z;
        y = -y;
        z = -z;

        dst_pose.key3d[i] = {x: x, y: y, z: z, score: s};
    }
}


function
render_3d_bone (gl, mtxGlobal, pose, idx0, idx1, color, rad, is_shadow)
{
    const pos0 = [pose.key3d[idx0].x, pose.key3d[idx0].y, pose.key3d[idx0].z];
    const pos1 = [pose.key3d[idx1].x, pose.key3d[idx1].y, pose.key3d[idx1].z];

    /* if the confidence score is low, draw more transparently. */
    const s0 =  0.9;//pose.key3d[idx0].score;
    const s1 =  0.9;//pose.key3d[idx1].score;
    const a  = color[3];

    color[3] = ((s0 > 0.1) && (s1 > 0.1)) ? a : 0.1;
    draw_bone (gl, mtxGlobal, pos0, pos1, rad, color, is_shadow);
    color[3] = a;
}

function
shadow_matrix (m, light_dir, ground_pos, ground_nrm)
{
    vec3_normalize (light_dir);
    vec3_normalize (ground_nrm);

    let a = ground_nrm[0];
    let b = ground_nrm[1];
    let c = ground_nrm[2];
    let d = 0;
    let ex = light_dir[0];
    let ey = light_dir[1];
    let ez = light_dir[2];

    m[ 0] =  b * ey + c * ez;
    m[ 1] = -a * ey;
    m[ 2] = -a * ez;
    m[ 3] = 0;

    m[ 4] = -b * ex;
    m[ 5] =  a * ex + c * ez;
    m[ 6] = -b * ez;
    m[ 7] = 0;

    m[ 8] = -c * ex;
    m[ 9] = -c * ey;
    m[10] =  a * ex + b * ey;
    m[11] = 0;

    m[12] = -d * ex;
    m[13] = -d * ey;
    m[14] = -d * ez;
    m[15] =  a * ex + b * ey + c * ey;
}

function 
render_skelton_3d (gl, landmarks)
{
    let mtxGlobal = new Array(16);
    let mtxTouch  = get_touch_event_matrix();
    let col_red    = [1.0, 0.0, 0.0, 1.0];
    let col_yellow = [1.0, 1.0, 0.0, 1.0];
    let col_green  = [0.0, 1.0, 0.0, 1.0];
    let col_cyan   = [0.0, 1.0, 1.0, 1.0];
    let col_violet = [1.0, 0.0, 1.0, 1.0];
    let col_blue   = [0.0, 0.5, 1.0, 1.0];
    let col_gray   = [0.0, 0.0, 0.0, 0.1];
    let col_node   = [1.0, 1.0, 1.0, 1.0];

    let pose_draw = {};
    pose_draw.key3d = [];
    compute_3d_skelton_pos (pose_draw, landmarks);

    let pose = pose_draw;
    for (let is_shadow = 1; is_shadow >= 0; is_shadow --)
    {
        let colj;
        let coln = col_node;

        matrix_identity (mtxGlobal);
        matrix_translate (mtxGlobal, 0.0, 0.0, -s_gui_prop.camera_pos_z);
        matrix_mult (mtxGlobal, mtxGlobal, mtxTouch);

        if (is_shadow)
        {
            let mtxShadow = new Array(16);
            let light_dir  = [1.0, 2.0, 1.0];
            let ground_pos = [0.0, 0.0, 0.0];
            let ground_nrm = [0.0, 1.0, 0.0];

            shadow_matrix (mtxShadow, light_dir, ground_pos, ground_nrm);

            let shadow_y = - s_gui_prop.pose_scale_y;
            //shadow_y += pose->key3d[kNeck].y * 0.5f;
            matrix_translate (mtxGlobal, 0.0, shadow_y, 0.0);
            matrix_mult (mtxGlobal, mtxGlobal, mtxShadow);

            colj = col_gray;
            coln = col_gray;
            colp = col_gray;
        }

        /* joint point */
        for (let i = 0; i < kPoseKeyNum; i ++)
        {
            const keyx = pose.key3d[i].x;
            const keyy = pose.key3d[i].y;
            const keyz = pose.key3d[i].z;
            const score= 1.0;

            const vec = [keyx, keyy, keyz];

            if (!is_shadow)
            {
                if      (i >= 13) colj = col_blue;  
                else if (i >= 12) colj = col_cyan;
                else if (i >=  10) colj = col_green;
                else if (i >=  9) colj = col_violet;
                else if (i >=  3) colj = col_red;
                else              colj = col_yellow;
            }

            const rad = (i < 19) ? s_gui_prop.joint_radius : s_gui_prop.joint_radius / 3;
            const alp = colj[3];
            colj[3] = (score > 0.1) ? alp : 0.1;
            draw_sphere (gl, mtxGlobal, vec, rad, colj, is_shadow);
            colj[3] = alp;
        }

        /* head and shoulder */
        const rad = s_gui_prop.bone_radius;
        render_3d_bone (gl, mtxGlobal, pose,  0,  1, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  0,  2, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  0,  3, coln, rad, is_shadow);

        //arm 
        render_3d_bone (gl, mtxGlobal, pose,  2,  4, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  3,  5, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  4,  6, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  5,  7, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  6,  8, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  7,  9, coln, rad, is_shadow);

        //body 
        render_3d_bone (gl, mtxGlobal, pose,  10,  11, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  0,  10, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  0,  11, coln, rad, is_shadow);

        //leg
        render_3d_bone (gl, mtxGlobal, pose,  10,  12, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  11,  13, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  12,  14, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  13,  15, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  14,  16, coln, rad, is_shadow);
        render_3d_bone (gl, mtxGlobal, pose,  15,  17, coln, rad, is_shadow);
    }
}


function render_3d_scene (gl, pose3d_predictions)
{
    let mtxGlobal = new Array(16);
    let mtxTouch  = get_touch_event_matrix();
    let floor_size_x = 300.0;
    let floor_size_y = 300.0;
    let floor_size_z = 300.0;

    /* background */
    matrix_identity (mtxGlobal);
    matrix_translate (mtxGlobal, 0, 0, -s_gui_prop.camera_pos_z);
    matrix_mult (mtxGlobal, mtxGlobal, mtxTouch);
    matrix_translate (mtxGlobal, 0, -s_gui_prop.pose_scale_y, 0);
    matrix_scale  (mtxGlobal, floor_size_x, floor_size_y, floor_size_z);
    matrix_translate (mtxGlobal, 0, 1.0, 0);
    draw_floor (gl, mtxGlobal, floor_size_x/10, floor_size_y/10);

    /* skelton */
    // for (let pose_id = 0; pose_id < pose3d_predictions.length; pose_id ++)
    // {
    const landmarks = pose3d_predictions.pose_landmark;
    render_skelton_3d (gl, landmarks);
    // }

    if (s_gui_prop.draw_axis)
    {
        /* (xyz)-AXIS */
        matrix_identity (mtxGlobal);
        matrix_translate (mtxGlobal, 0, 0, -s_gui_prop.camera_pos_z);
        matrix_mult (mtxGlobal, mtxGlobal, mtxTouch);
        for (let i = -1; i <= 1; i ++)
        {
            for (let j = -1; j <= 1; j ++)
            {
                let colb = [0.1, 0.5, 0.5, 0.5];
                let dx = s_gui_prop.pose_scale_x;
                let dy = s_gui_prop.pose_scale_y;
                let dz = s_gui_prop.pose_scale_z;
                let rad = 1;
                let v0 = [];
                let v1 = [];

                v0  = [-dx, i * dy, j * dz];
                v1  = [ dx, i * dy, j * dz];

                col = colb;
                if (i == 0 && j == 0)
                    col = [1.0, 0.0, 0.0, 1.0];
                draw_line (gl, mtxGlobal, v0, v1, col);
                draw_sphere (gl, mtxGlobal, v1, rad, col, 0);

                v0  = [i * dx, -dy, j * dz];
                v1  = [i * dx,  dy, j * dz];
                col = colb;
                if (i == 0 && j == 0)
                    col = [0.0, 1.0, 0.0, 1.0];
                draw_line (gl, mtxGlobal, v0, v1, col);
                draw_sphere (gl, mtxGlobal, v1, rad, col, 0);

                v0  = [i * dx, j * dy, -dz];
                v1  = [i * dx, j * dy,  dz];
                col = colb;
                if (i == 0 && j == 0)
                    col = [0.0, 0.0, 1.0, 1.0];
                draw_line (gl, mtxGlobal, v0, v1, col);
                draw_sphere (gl, mtxGlobal, v1, rad, col, 0);
            }
        }
    }
}

function render_2d_bone (gl, ofstx, ofsty, drw_w, drw_h, pose_ret, pid, id0, id1, col)
{
    const x0 = pose_ret[pid].key[id0].x * drw_w + ofstx;
    const y0 = pose_ret[pid].key[id0].y * drw_h + ofsty;
    const x1 = pose_ret[pid].key[id1].x * drw_w + ofstx;
    const y1 = pose_ret[pid].key[id1].y * drw_h + ofsty;
    const s0 = pose_ret[pid].key[id0].score;
    const s1 = pose_ret[pid].key[id1].score;

    /* if the confidence score is low, draw more transparently. */
    col[3] = (s0 + s1) * 0.5;
    r2d.draw_2d_line (gl, x0, y0, x1, y1, col, 5.0);
    col[3] = 1.0;
}

function 
render_2d_scene (gl, texid, pose_ret)
{
    const col_red    = [1.0, 0.0, 0.0, 1.0];
    const col_yellow = [1.0, 1.0, 0.0, 1.0];
    const col_green  = [0.0, 1.0, 0.0, 1.0];
    const col_cyan   = [0.0, 1.0, 1.0, 1.0];
    const col_violet = [1.0, 0.0, 1.0, 1.0];
    const col_blue   = [0.0, 0.5, 1.0, 1.0];

    let color = [0.0, 1.0, 1.0, 1.0]
    let scale  = s_gui_prop.srcimg_scale;
    let tx = 5;
    let ty = 60;
    let tw = s_srctex_region.tex_w * scale;
    let th = s_srctex_region.tex_h * scale;
    let x = -s_srctex_region.tex_x * scale + tx;
    let y = -s_srctex_region.tex_y * scale + ty;
    let w =  s_srctex_region.width * scale;
    let h =  s_srctex_region.height * scale;

    r2d.draw_2d_texture (gl, texid, tx, ty, tw, th, 0)
    r2d.draw_2d_rect (gl, tx, ty, tw, th, [0.0, 1.0, 1.0, 1.0], 3.0);

    for (let i = 0; i < pose_ret.length; i++)
    {
        /* right arm */
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  1,  2, col_red);
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  2,  3, col_red);
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  3,  4, col_red);

        /* left arm */
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  1,  5, col_violet);
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  5,  6, col_violet);
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  6,  7, col_violet);

        /* right leg */
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  1,  8, col_green);
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  8,  9, col_green);
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  9, 10, col_green);

        /* left leg */
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  1, 11, col_cyan);
        render_2d_bone (gl, x, y, w, h, pose_ret, i, 11, 12, col_cyan);
        render_2d_bone (gl, x, y, w, h, pose_ret, i, 12, 13, col_cyan);

        /* neck */
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  1,  0, col_yellow);

        /* eye */
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  0, 14, col_blue);
        render_2d_bone (gl, x, y, w, h, pose_ret, i, 14, 16, col_blue);
        render_2d_bone (gl, x, y, w, h, pose_ret, i,  0, 15, col_blue);
        render_2d_bone (gl, x, y, w, h, pose_ret, i, 15, 17, col_blue);

        /* draw key points */
        for (let j = 0; j < kPoseKeyNum - 1; j++)
        {
            let  colj;
            if      (j >= 14) colj = col_blue;
            else if (j >= 11) colj = col_cyan;
            else if (j >=  8) colj = col_green;
            else if (j >=  5) colj = col_violet;
            else if (j >=  2) colj = col_red;
            else              colj = col_yellow;

            const keyx = pose_ret[i].key[j].x * w + x;
            const keyy = pose_ret[i].key[j].y * h + y;
            const score= pose_ret[i].key[j].score;

            let r = 9 * scale;
            colj[3] = score;
            r2d.draw_2d_fillrect (gl, keyx - (r/2), keyy - (r/2), r, r, colj);
            colj[3] = 1.0;
        }
    }
}


function on_resize (gl)
{
    let w = gl.canvas.width;
    let h = gl.canvas.height;

    gl.viewport (0, 0, w, h);
    pmeter.resize (gl, w, h, h - 100);
    dbgstr.resize_viewport (gl, w, h);
    r2d.resize_viewport (gl, w, h);
    resize_pose3d_render (gl, w, h);

    GLUtil.destroy_render_target (gl, s_rtarget_main);
    GLUtil.destroy_render_target (gl, s_rtarget_feed);
    s_rtarget_main = GLUtil.create_render_target (gl, w, h, 0);
    s_rtarget_feed = GLUtil.create_render_target (gl, w, h, 1);
}

function check_resize_canvas (gl, canvas)
{
    let display_w = canvas.clientWidth;
    let display_h = canvas.clientHeight;

    if (canvas.width  != display_w ||
        canvas.height != display_h) 
    {
        canvas.width  = display_w;
        canvas.height = display_h;
        on_resize (gl);
    }
}


function
init_gui ()
{
    const gui = new dat.GUI();

    gui.add (s_gui_prop, 'pose_scale_x', 0, 1000);
    gui.add (s_gui_prop, 'pose_scale_y', 0, 1000);
    gui.add (s_gui_prop, 'pose_scale_z', 0, 1000);
    gui.add (s_gui_prop, 'camera_pos_z', 0, 1000);
    gui.add (s_gui_prop, 'joint_radius', 0, 20);
    gui.add (s_gui_prop, 'bone_radius',  0, 20);
    gui.add (s_gui_prop, 'srcimg_scale', 0, 5.0);
    gui.add (s_gui_prop, 'draw_axis');
    gui.add (s_gui_prop, 'draw_pmeter');
}

function createSensorClient() {
    //    var client = new WebSocketClient();
    // var count = 0
    // client.on('connectFailed', function(error) {
    //     console.log('Connect Error: ' + error.toString());
    // });

    // client.on('connect', function(connection) {
    //     console.log('WebSocket Client Connected');
    //     connection.on('error', function(error) {
    //         console.log("Connection Error: " + error.toString());
    //     });
    //     connection.on('close', function() {
    //         console.log('echo-protocol Connection Closed');
    //     });
    //     connection.on('message', function(message) {
    //         if (message.type === 'utf8') {
    //             var profilingTime = JSON.parse(message.utf8Data).timeProfiling 
    //             profilingTime.clientReceiveTime = Date.now()
    //             test_landmark_p = JSON.parse(message.utf8Data)
    //             console.log(profilingTime);
    //         }

    //     });
    //     var appClientMessage =  {
    //         "type" : "application_client"
    //     }
    //     var groundLocationReset = {
    //         "type" : "application_control",
    //         "feature_id" : "ground_location",
    //         "action" : "reset" 
    //     }
    //     connection.send(JSON.stringify(appClientMessage));
    // });

    // client.connect('ws://localhost:8181/');

  var client = new WebSocket('ws://127.0.0.1:8181/');

  // Connection opened
  client.addEventListener('open', function (event) {
    console.log("open websocket")

    var appClientMessage = {
        "type" : "application_client"
    }
    client.send(JSON.stringify(appClientMessage))
  });

  // Connection opened
  client.addEventListener('error', function (event) {
    console.log("error websocket")
    console.log(event)
  });

  // Connection opened
  client.addEventListener('close', function (event) {
    console.log("close websocket")
    console.log(event)
    client = new WebSocket('ws://127.0.0.1:8181/');
  });

  client.addEventListener('message', function(event) {
    var profilingTime = JSON.parse(event.data).timeProfiling 
    profilingTime.clientReceiveTime = Date.now()
    //move 3d from fk to landmarks
    test_landmark_p = JSON.parse(event.data)
    test_landmark_p.pose_landmark.keypoints3D = test_landmark_p.fitting.keypoints3D
    // console.log(profilingTime);   
  })

  function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
  return client
}



/* ---------------------------------------------------------------- *
 *      M A I N    F U N C T I O N
 * ---------------------------------------------------------------- */
async function startWebGL()
{
    s_debug_log = document.getElementById('debug_log');
    let current_phase = 0;

    const canvas = document.querySelector('#glcanvas');
    const gl = canvas.getContext('webgl');
    if (!gl)
    {
        alert('Failed to initialize WebGL.');
        return;
    }

    gl.clearColor (0.7, 0.7, 0.7, 1.0);
    gl.clear (gl.COLOR_BUFFER_BIT);

    init_touch_event (canvas);
    init_gui ();

    // const camtex = GLUtil.create_camera_texture (gl);
    //const camtex = GLUtil.create_video_texture (gl, "./assets/just_do_it.mp4");
    // const imgtex = GLUtil.create_image_texture2 (gl, "pakutaso_person.jpg");

    let win_w = canvas.clientWidth;
    let win_h = canvas.clientHeight;

    r2d.init_2d_render (gl, win_w, win_h);
    init_pose3d_render (gl, win_w, win_h);

    init_dbgstr (gl, win_w, win_h);
    pmeter.init_pmeter (gl, win_w, win_h, win_h - 40);
    //const stats = init_stats ();


    // await init_tfjs_pose3d ();
    //s_debug_log.innerHTML = "tfjs.Backend = " + tf.getBackend() + "<br>"

    s_rtarget_main = GLUtil.create_render_target (gl, win_w, win_h, 0);
    s_rtarget_feed = GLUtil.create_render_target (gl, win_w, win_w, 1);

    /* stop loading spinner */
    const spinner = document.getElementById('loading');
    spinner.classList.add('loaded');

    let prev_time_ms = performance.now();
    async function render (now)
    {
        pmeter.reset_lap (0);
        pmeter.set_lap (0);

        let cur_time_ms = performance.now();
        let interval_ms = cur_time_ms - prev_time_ms;
        prev_time_ms = cur_time_ms;

        //stats.begin();

        check_resize_canvas (gl, canvas);
        win_w = canvas.width;
        win_h = canvas.height;

        // let src_w = imgtex.image.width;
        // let src_h = imgtex.image.height;
        // let texid = imgtex.texid;
        // if (GLUtil.is_camera_ready(camtex))
        // {
        //     GLUtil.update_camera_texture (gl, camtex);
        //     src_w = camtex.video.videoWidth;
        //     src_h = camtex.video.videoHeight;
        //     texid = camtex.texid;
        // }

        /* --------------------------------------- *
         *  invoke TF.js (Pose detection)
         * --------------------------------------- */
        // let feed_image = generate_input_image (gl, texid, src_w, src_h, win_w, win_h);
        // let pose3d_predictions = {length: 0};

        let time_invoke0_start = performance.now();
        // pose3d_predictions = await await invoke_pose_detect (feed_image);
        let time_invoke0 = performance.now() - time_invoke0_start;


        /* --------------------------------------- *
         *  render scene
         * --------------------------------------- */
        GLUtil.set_render_target (gl, s_rtarget_main);
        gl.clear (gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        render_3d_scene (gl, test_landmark_p);
        // render_2d_scene (gl, texid, testPose);

        /* --------------------------------------- *
         *  post process
         * --------------------------------------- */
        if (s_gui_prop.draw_pmeter)
        {
            pmeter.draw_pmeter (gl, 0, 40);
        }

        let str = "Interval: " + interval_ms.toFixed(1) + " [ms]";
        dbgstr.draw_dbgstr (gl, str, 10, 10);

        str = "TF.js0  : " + time_invoke0.toFixed(1)  + " [ms]";
        dbgstr.draw_dbgstr (gl, str, 10, 10 + 22 * 1);

        str = "BACKEND: " + tf.getBackend();
        dbgstr.draw_dbgstr_ex (gl, str, win_w - 220, win_h - 22 * 3, 
            1, [0.0, 1.0, 1.0, 1.0], [0.2, 0.2, 0.2, 1.0]);

        str = "window(" + win_w + ", " + win_h + ")";
        dbgstr.draw_dbgstr (gl, str, win_w - 220, win_h - 22 * 2);

        // str = "srcdim(" + src_w + ", " + src_h + ")";
        // dbgstr.draw_dbgstr (gl, str, win_w - 220, win_h - 22 * 1);


        //stats.end();
        requestAnimationFrame (render);
    }
    createSensorClient()
    render ();
}


var testPose = [{
    "key": [
        {
            "x": 0.4909090909090909,
            "y": 0.7741935483870968,
            "score": 0.6989794969558716
        },
        {
            "x": 0.4727272727272727,
            "y": 1,
            "score": 0.9
        },
        {
            "x": 0.2909090909090909,
            "y": 1,
            "score": 0.9
        },
        {
            "x": 0.45454545454545453,
            "y": 0.967741935483871,
            "score": 0.9
        },
        {
            "x": 0.6181818181818182,
            "y": 0.6129032258064516,
            "score": 0.9
        },
        {
            "x": 0.6727272727272727,
            "y": 1,
            "score": 0.9
        },
        {
            "x": 0.5272727272727272,
            "y": 0.8064516129032258,
            "score": 0.9
        },
        {
            "x": 0.6181818181818182,
            "y": 0.6129032258064516,
            "score": 0.9
        },
        {
            "x": 0.41818181818181815,
            "y": 0.6129032258064516,
            "score": 0.9
        },
        {
            "x": 0.5454545454545454,
            "y": 0.6451612903225806,
            "score": 0.9
        },
        {
            "x": 0.6545454545454545,
            "y": 0.6451612903225806,
            "score": 0.9
        },
        {
            "x": 0.509090909090909,
            "y": 0.7096774193548387,
            "score": 0.9
        },
        {
            "x": 0.5272727272727272,
            "y": 0.6774193548387096,
            "score": 0.9
        },
        {
            "x": 0.6545454545454545,
            "y": 0.6451612903225806,
            "score": 0.9
        },
        {
            "x": 0.43636363636363634,
            "y": 0.6774193548387096,
            "score": 0.9
        },
        {
            "x": 0.5454545454545454,
            "y": 0.6774193548387096,
            "score": 0.9
        },
        {
            "x": 0.38181818181818183,
            "y": 0.7096774193548387,
            "score": 0.9
        },
        {
            "x": 0.6,
            "y": 0.7096774193548387,
            "score": 0.9
        },
        {
            "x": 0.6363636363636364,
            "y": 0.9032258064516129,
            "score": 1.0043089389801025
        }
    ],
    "key3d": [
        {
            "x": -0.01628926396369934,
            "y": -0.05157969519495964,
            "z": -0.08754239976406097,
            "score": 0.9
        },
        {
            "x": 0.00003536255098879337,
            "y": 0.0002952466020360589,
            "z": -0.00016889828839339316,
            "score": 0.9
        },
        {
            "x": 0.042143817991018295,
            "y": -0.0038793282583355904,
            "z": -0.0030180118046700954,
            "score": 0.9
        },
        {
            "x": -0.05212414264678955,
            "y": 0.13869456946849823,
            "z": 0.036507438868284225,
            "score": 0.9
        },
        {
            "x": -0.02182089537382126,
            "y": 0.172099307179451,
            "z": 0.056185122579336166,
            "score": 0.9
        },
        {
            "x": -0.03401470556855202,
            "y": 0.006997283548116684,
            "z": -0.03325197473168373,
            "score": 0.9
        },
        {
            "x": 0.11122957617044449,
            "y": 0.12018970400094986,
            "z": 0.028416702523827553,
            "score": 0.9
        },
        {
            "x": 0.07142821699380875,
            "y": 0.1773877888917923,
            "z": 0.04686879366636276,
            "score": 0.9
        },
        {
            "x": -0.05535643547773361,
            "y": 0.242206871509552,
            "z": 0.1139674186706543,
            "score": 0.9
        },
        {
            "x": -0.06708516925573349,
            "y": 0.4137594997882843,
            "z": 0.09687938541173935,
            "score": 0.9
        },
        {
            "x": -0.014392219483852386,
            "y": 0.612252414226532,
            "z": 0.2261328548192978,
            "score": 0.9
        },
        {
            "x": 0.029351284727454185,
            "y": 0.2475539743900299,
            "z": 0.07880465686321259,
            "score": 0.9
        },
        {
            "x": -0.009461330249905586,
            "y": 0.4288991689682007,
            "z": 0.08067522943019867,
            "score": 0.9
        },
        {
            "x": 0.016652189195156097,
            "y": 0.6128856539726257,
            "z": 0.21099892258644104,
            "score": 0.9
        },
        {
            "x": 0.026638180017471313,
            "y": -0.0384824275970459,
            "z": -0.019383065402507782,
            "score": 0.9
        },
        {
            "x": 0.003649923950433731,
            "y": 0.1519305557012558,
            "z": 0.21935726702213287,
            "score": 0.9
        },
        {
            "x": 0.027276530861854553,
            "y": -0.08707275241613388,
            "z": -0.07081997394561768,
            "score": 0.9
        },
        {
            "x": -0.04511412978172302,
            "y": 0.004158448427915573,
            "z": 0.08032017201185226,
            "score": 0.9
        },
        {
            "x": 0.011685416102409363,
            "y": 0.27222904562950134,
            "z": 0.05404074862599373,
            "score": 1.0043089389801025
        }
    ],
    "pose_score": 1
}]


var test_landmark_p = {"timeProfiling":{"startTime":1652507872630,"afterPoseDetection":1652507872658,"beforeSend":1652507872658,"serverReceive":1652507872658,"processingTime":1652507872658,"beforeSendTime":1652507872659},"type":"application_frame","monitor":{"rawData_z":-0.0003487006761133671,"watchData_z":0.9660811035957806,"rawData_x":0.5142865180969238,"watchData_x":0.04080417152209773,"rawData_y":0.4785833954811096,"watchData_y":1.0982997150317295},"ground_location":{"x":0.01956155825351081,"y":1.0982997150317295,"z":0.5928372002904859,"legLength":0.8588251071050763,"tracing":true},"action_detection":{"version":"0.1.0","walk":{"legUp":1,"frequency":280,"strength":0.07496607692201618},"jump":{"up":0,"strength":0}},"gaze_tracking":{"x":-0.03742461501337736,"y":0.20179304458566427,"z":1.246886438515923},"pose_landmark":{"keypoints":[{"x":0.4400736093521118,"y":0.1966688334941864,"z":-0.6143686175346375,"score":0.9998576045036316,"name":"nose"},{"x":0.44830504059791565,"y":0.1814088523387909,"z":-0.573645293712616,"score":0.9996354579925537,"name":"left_eye_inner"},{"x":0.4632866680622101,"y":0.18073846399784088,"z":-0.5741636753082275,"score":0.9996238350868225,"name":"left_eye"},{"x":0.4733141362667084,"y":0.18005795776844025,"z":-0.5740962624549866,"score":0.9997150301933289,"name":"left_eye_outer"},{"x":0.41737252473831177,"y":0.18263740837574005,"z":-0.579922080039978,"score":0.9997201561927795,"name":"right_eye_inner"},{"x":0.4082501232624054,"y":0.18339934945106506,"z":-0.5808301568031311,"score":0.999769389629364,"name":"right_eye"},{"x":0.3973217010498047,"y":0.1845480501651764,"z":-0.5812891721725464,"score":0.9997948408126831,"name":"right_eye_outer"},{"x":0.4838116765022278,"y":0.18698136508464813,"z":-0.32197585701942444,"score":0.9997860789299011,"name":"left_ear"},{"x":0.38386204838752747,"y":0.19508129358291626,"z":-0.3520677983760834,"score":0.9995032548904419,"name":"right_ear"},{"x":0.4653974771499634,"y":0.21210971474647522,"z":-0.5126641392707825,"score":0.9998378157615662,"name":"mouth_left"},{"x":0.42781883478164673,"y":0.21467043459415436,"z":-0.5263512134552002,"score":0.9998146295547485,"name":"mouth_right"},{"x":0.5874736905097961,"y":0.27407151460647583,"z":-0.16848811507225037,"score":0.9999825358390808,"name":"left_shoulder"},{"x":0.32966992259025574,"y":0.2903103828430176,"z":-0.20243272185325623,"score":0.9999414682388306,"name":"right_shoulder"},{"x":0.6531283259391785,"y":0.38517943024635315,"z":-0.05138421058654785,"score":0.9836919903755188,"name":"left_elbow"},{"x":0.29463115334510803,"y":0.39755943417549133,"z":-0.2149823009967804,"score":0.9633356332778931,"name":"right_elbow"},{"x":0.6973592638969421,"y":0.4860007166862488,"z":-0.16037710011005402,"score":0.9888449311256409,"name":"left_wrist"},{"x":0.34518250823020935,"y":0.4951816499233246,"z":-0.4148649573326111,"score":0.9840887188911438,"name":"right_wrist"},{"x":0.7004298567771912,"y":0.5159487724304199,"z":-0.2015930414199829,"score":0.9853824377059937,"name":"left_pinky"},{"x":0.3672032356262207,"y":0.5250412821769714,"z":-0.4786522090435028,"score":0.9800065159797668,"name":"right_pinky"},{"x":0.6803839802742004,"y":0.5152179598808289,"z":-0.25485748052597046,"score":0.9866468906402588,"name":"left_index"},{"x":0.38701289892196655,"y":0.5169064998626709,"z":-0.5270803570747375,"score":0.9811820387840271,"name":"right_index"},{"x":0.6710778474807739,"y":0.5074383616447449,"z":-0.17801018059253693,"score":0.9830063581466675,"name":"left_thumb"},{"x":0.3857783377170563,"y":0.506790041923523,"z":-0.43881386518478394,"score":0.9765325784683228,"name":"right_thumb"},{"x":0.587433397769928,"y":0.51656174659729,"z":0.01364580262452364,"score":0.9999662041664124,"name":"left_hip"},{"x":0.4411396384239197,"y":0.5262714624404907,"z":-0.014343203976750374,"score":0.9999516010284424,"name":"right_hip"},{"x":0.6139312386512756,"y":0.6711891293525696,"z":0.045111339539289474,"score":0.9767786860466003,"name":"left_knee"},{"x":0.45901983976364136,"y":0.6834450960159302,"z":-0.07816039770841599,"score":0.9608855247497559,"name":"right_knee"},{"x":0.6244639754295349,"y":0.7919110059738159,"z":0.4074709713459015,"score":0.9747146964073181,"name":"left_ankle"},{"x":0.477110356092453,"y":0.7992194890975952,"z":0.28717178106307983,"score":0.9391025900840759,"name":"right_ankle"},{"x":0.6096153259277344,"y":0.8079610466957092,"z":0.4305192530155182,"score":0.8536874055862427,"name":"left_heel"},{"x":0.49853527545928955,"y":0.8147327303886414,"z":0.3165687322616577,"score":0.7939006090164185,"name":"right_heel"},{"x":0.6359513998031616,"y":0.8685532808303833,"z":0.21415439248085022,"score":0.9682245850563049,"name":"left_foot_index"},{"x":0.467390239238739,"y":0.8717108368873596,"z":0.1084616482257843,"score":0.9328898191452026,"name":"right_foot_index"}],"keypoints3D":[{"x":-0.08908429741859436,"y":-0.6056130528450012,"z":-0.34427669644355774,"score":0.9998576045036316,"name":"nose"},{"x":-0.07008104771375656,"y":-0.6310279965400696,"z":-0.3353848457336426,"score":0.9996354579925537,"name":"left_eye_inner"},{"x":-0.06865458190441132,"y":-0.630757749080658,"z":-0.32351553440093994,"score":0.9996238350868225,"name":"left_eye"},{"x":-0.06965970247983932,"y":-0.6307281851768494,"z":-0.327378511428833,"score":0.9997150301933289,"name":"left_eye_outer"},{"x":-0.09566515684127808,"y":-0.641137957572937,"z":-0.34654858708381653,"score":0.9997201561927795,"name":"right_eye_inner"},{"x":-0.09516496956348419,"y":-0.638408899307251,"z":-0.35856086015701294,"score":0.999769389629364,"name":"right_eye"},{"x":-0.08818478882312775,"y":-0.6262077689170837,"z":-0.3387161195278168,"score":0.9997948408126831,"name":"right_eye_outer"},{"x":-0.00558169512078166,"y":-0.6348605155944824,"z":-0.2256198525428772,"score":0.9997860789299011,"name":"left_ear"},{"x":-0.14362049102783203,"y":-0.580715000629425,"z":-0.22572682797908783,"score":0.9995032548904419,"name":"right_ear"},{"x":-0.051925234496593475,"y":-0.6041181087493896,"z":-0.2923634946346283,"score":0.9998378157615662,"name":"mouth_left"},{"x":-0.0854174867272377,"y":-0.5679923295974731,"z":-0.31773504614830017,"score":0.9998146295547485,"name":"mouth_right"},{"x":0.10929055511951447,"y":-0.4899207055568695,"z":-0.1038556918501854,"score":0.9999825358390808,"name":"left_shoulder"},{"x":-0.21117526292800903,"y":-0.5167290568351746,"z":-0.05424673855304718,"score":0.9999414682388306,"name":"right_shoulder"},{"x":0.19826024770736694,"y":-0.2790902554988861,"z":-0.06712137162685394,"score":0.9836919903755188,"name":"left_elbow"},{"x":-0.22890391945838928,"y":-0.2300361841917038,"z":-0.0790054053068161,"score":0.9633356332778931,"name":"right_elbow"},{"x":0.23067763447761536,"y":-0.05368216335773468,"z":-0.03209317848086357,"score":0.9888449311256409,"name":"left_wrist"},{"x":-0.18979157507419586,"y":-0.04373640939593315,"z":-0.14424103498458862,"score":0.9840887188911438,"name":"right_wrist"},{"x":0.22908668220043182,"y":-0.0025944914668798447,"z":-0.025060750544071198,"score":0.9853824377059937,"name":"left_pinky"},{"x":-0.15517506003379822,"y":-0.004136913921684027,"z":-0.1859588325023651,"score":0.9800065159797668,"name":"right_pinky"},{"x":0.19878631830215454,"y":0.010257037356495857,"z":-0.04854492470622063,"score":0.9866468906402588,"name":"left_index"},{"x":-0.11446545273065567,"y":-0.03792218491435051,"z":-0.20294728875160217,"score":0.9811820387840271,"name":"right_index"},{"x":0.22660033404827118,"y":-0.021178539842367172,"z":-0.03827113285660744,"score":0.9830063581466675,"name":"left_thumb"},{"x":-0.16878165304660797,"y":-0.04581800475716591,"z":-0.15905341506004333,"score":0.9765325784683228,"name":"right_thumb"},{"x":0.09591226279735565,"y":-0.005513381212949753,"z":0.02492908574640751,"score":0.9999662041664124,"name":"left_hip"},{"x":-0.0930781438946724,"y":0.002795955864712596,"z":-0.02325385995209217,"score":0.9999516010284424,"name":"right_hip"},{"x":0.1303819715976715,"y":0.39518222212791443,"z":0.002974419854581356,"score":0.9767786860466003,"name":"left_knee"},{"x":-0.061457399278879166,"y":0.3270227313041687,"z":-0.018467534333467484,"score":0.9608855247497559,"name":"right_knee"},{"x":0.15117432177066803,"y":0.6855847835540771,"z":0.19995945692062378,"score":0.9747146964073181,"name":"left_ankle"},{"x":-0.06294907629489899,"y":0.6757419109344482,"z":0.18194463849067688,"score":0.9391025900840759,"name":"right_ankle"},{"x":0.13217630982398987,"y":0.7210068106651306,"z":0.22476904094219208,"score":0.8536874055862427,"name":"left_heel"},{"x":-0.06394592672586441,"y":0.7057092189788818,"z":0.16263200342655182,"score":0.7939006090164185,"name":"right_heel"},{"x":0.1372155249118805,"y":0.7860890030860901,"z":0.18649837374687195,"score":0.9682245850563049,"name":"left_foot_index"},{"x":-0.07849862426519394,"y":0.7605543732643127,"z":0.10873527824878693,"score":0.9328898191452026,"name":"right_foot_index"}],"timestamp":1652507872659,"version":"0.1.0"}}