"use strict";
// automatically generated by the FlatBuffers compiler, do not modify
exports.__esModule = true;
exports.Jump = void 0;
var flatbuffers = require("/Users/nixiaofeng/develop/feature/os_dev_docker/app/src/wasm/flatbuffers/flatbuffers.js");
var Jump = /** @class */ (function () {
    function Jump() {
        this.bb = null;
        this.bb_pos = 0;
    }
    Jump.prototype.__init = function (i, bb) {
        this.bb_pos = i;
        this.bb = bb;
        return this;
    };
    Jump.getRootAsJump = function (bb, obj) {
        return (obj || new Jump()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    Jump.getSizePrefixedRootAsJump = function (bb, obj) {
        bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
        return (obj || new Jump()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    Jump.prototype.up = function () {
        var offset = this.bb.__offset(this.bb_pos, 4);
        return offset ? this.bb.readFloat32(this.bb_pos + offset) : 0.0;
    };
    Jump.prototype.strength = function () {
        var offset = this.bb.__offset(this.bb_pos, 6);
        return offset ? this.bb.readFloat32(this.bb_pos + offset) : 0.0;
    };
    Jump.startJump = function (builder) {
        builder.startObject(2);
    };
    Jump.addUp = function (builder, up) {
        builder.addFieldFloat32(0, up, 0.0);
    };
    Jump.addStrength = function (builder, strength) {
        builder.addFieldFloat32(1, strength, 0.0);
    };
    Jump.endJump = function (builder) {
        var offset = builder.endObject();
        return offset;
    };
    Jump.createJump = function (builder, up, strength) {
        Jump.startJump(builder);
        Jump.addUp(builder, up);
        Jump.addStrength(builder, strength);
        return Jump.endJump(builder);
    };
    return Jump;
}());
exports.Jump = Jump;
