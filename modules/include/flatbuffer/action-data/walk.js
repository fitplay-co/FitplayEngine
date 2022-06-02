"use strict";
// automatically generated by the FlatBuffers compiler, do not modify
exports.__esModule = true;
exports.Walk = void 0;
var flatbuffers = require("/Users/nixiaofeng/develop/feature/os_dev_docker/app/src/wasm/flatbuffers/flatbuffers.js");
var Walk = /** @class */ (function () {
    function Walk() {
        this.bb = null;
        this.bb_pos = 0;
    }
    Walk.prototype.__init = function (i, bb) {
        this.bb_pos = i;
        this.bb = bb;
        return this;
    };
    Walk.getRootAsWalk = function (bb, obj) {
        return (obj || new Walk()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    Walk.getSizePrefixedRootAsWalk = function (bb, obj) {
        bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
        return (obj || new Walk()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    Walk.prototype.legUp = function () {
        var offset = this.bb.__offset(this.bb_pos, 4);
        return offset ? this.bb.readFloat32(this.bb_pos + offset) : 0.0;
    };
    Walk.prototype.frequency = function () {
        var offset = this.bb.__offset(this.bb_pos, 6);
        return offset ? this.bb.readFloat32(this.bb_pos + offset) : 0.0;
    };
    Walk.prototype.strength = function () {
        var offset = this.bb.__offset(this.bb_pos, 8);
        return offset ? this.bb.readFloat32(this.bb_pos + offset) : 0.0;
    };
    Walk.startWalk = function (builder) {
        builder.startObject(3);
    };
    Walk.addLegUp = function (builder, legUp) {
        builder.addFieldFloat32(0, legUp, 0.0);
    };
    Walk.addFrequency = function (builder, frequency) {
        builder.addFieldFloat32(1, frequency, 0.0);
    };
    Walk.addStrength = function (builder, strength) {
        builder.addFieldFloat32(2, strength, 0.0);
    };
    Walk.endWalk = function (builder) {
        var offset = builder.endObject();
        return offset;
    };
    Walk.createWalk = function (builder, legUp, frequency, strength) {
        Walk.startWalk(builder);
        Walk.addLegUp(builder, legUp);
        Walk.addFrequency(builder, frequency);
        Walk.addStrength(builder, strength);
        return Walk.endWalk(builder);
    };
    return Walk;
}());
exports.Walk = Walk;
