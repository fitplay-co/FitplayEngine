"use strict";
// automatically generated by the FlatBuffers compiler, do not modify
exports.__esModule = true;
exports.Action = void 0;
var flatbuffers = require("../../../../app/src/wasm/flatbuffers/flatbuffers");
var gaze_1 = require("../action-data/gaze");
var ground_1 = require("../action-data/ground");
var joint_1 = require("../action-data/joint");
var jump_1 = require("../action-data/jump");
var walk_1 = require("../action-data/walk");
var Action = /** @class */ (function () {
    function Action() {
        this.bb = null;
        this.bb_pos = 0;
    }
    Action.prototype.__init = function (i, bb) {
        this.bb_pos = i;
        this.bb = bb;
        return this;
    };
    Action.getRootAsAction = function (bb, obj) {
        return (obj || new Action()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    Action.getSizePrefixedRootAsAction = function (bb, obj) {
        bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
        return (obj || new Action()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    Action.prototype.walk = function (obj) {
        var offset = this.bb.__offset(this.bb_pos, 4);
        return offset ? (obj || new walk_1.Walk()).__init(this.bb.__indirect(this.bb_pos + offset), this.bb) : null;
    };
    Action.prototype.jump = function (obj) {
        var offset = this.bb.__offset(this.bb_pos, 6);
        return offset ? (obj || new jump_1.Jump()).__init(this.bb.__indirect(this.bb_pos + offset), this.bb) : null;
    };
    Action.prototype.gaze = function (obj) {
        var offset = this.bb.__offset(this.bb_pos, 8);
        return offset ? (obj || new gaze_1.Gaze()).__init(this.bb.__indirect(this.bb_pos + offset), this.bb) : null;
    };
    Action.prototype.ground = function (obj) {
        var offset = this.bb.__offset(this.bb_pos, 10);
        return offset ? (obj || new ground_1.Ground()).__init(this.bb.__indirect(this.bb_pos + offset), this.bb) : null;
    };
    Action.prototype.joints = function (index, obj) {
        var offset = this.bb.__offset(this.bb_pos, 12);
        return offset ? (obj || new joint_1.Joint()).__init(this.bb.__indirect(this.bb.__vector(this.bb_pos + offset) + index * 4), this.bb) : null;
    };
    Action.prototype.jointsLength = function () {
        var offset = this.bb.__offset(this.bb_pos, 12);
        return offset ? this.bb.__vector_len(this.bb_pos + offset) : 0;
    };
    Action.startAction = function (builder) {
        builder.startObject(5);
    };
    Action.addWalk = function (builder, walkOffset) {
        builder.addFieldOffset(0, walkOffset, 0);
    };
    Action.addJump = function (builder, jumpOffset) {
        builder.addFieldOffset(1, jumpOffset, 0);
    };
    Action.addGaze = function (builder, gazeOffset) {
        builder.addFieldOffset(2, gazeOffset, 0);
    };
    Action.addGround = function (builder, groundOffset) {
        builder.addFieldOffset(3, groundOffset, 0);
    };
    Action.addJoints = function (builder, jointsOffset) {
        builder.addFieldOffset(4, jointsOffset, 0);
    };
    Action.createJointsVector = function (builder, data) {
        builder.startVector(4, data.length, 4);
        for (var i = data.length - 1; i >= 0; i--) {
            builder.addOffset(data[i]);
        }
        return builder.endVector();
    };
    Action.startJointsVector = function (builder, numElems) {
        builder.startVector(4, numElems, 4);
    };
    Action.endAction = function (builder) {
        var offset = builder.endObject();
        return offset;
    };
    Action.finishActionBuffer = function (builder, offset) {
        builder.finish(offset);
    };
    Action.finishSizePrefixedActionBuffer = function (builder, offset) {
        builder.finish(offset, undefined, true);
    };
    return Action;
}());
exports.Action = Action;
