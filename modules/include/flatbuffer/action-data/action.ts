// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { Gaze } from '../action-data/gaze';
import { Ground } from '../action-data/ground';
import { Jump } from '../action-data/jump';
import { Walk } from '../action-data/walk';


export class Action {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
__init(i:number, bb:flatbuffers.ByteBuffer):Action {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsAction(bb:flatbuffers.ByteBuffer, obj?:Action):Action {
  return (obj || new Action()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsAction(bb:flatbuffers.ByteBuffer, obj?:Action):Action {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new Action()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

walk(obj?:Walk):Walk|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? (obj || new Walk()).__init(this.bb!.__indirect(this.bb_pos + offset), this.bb!) : null;
}

jump(obj?:Jump):Jump|null {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? (obj || new Jump()).__init(this.bb!.__indirect(this.bb_pos + offset), this.bb!) : null;
}

gaze(obj?:Gaze):Gaze|null {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? (obj || new Gaze()).__init(this.bb!.__indirect(this.bb_pos + offset), this.bb!) : null;
}

ground(obj?:Ground):Ground|null {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? (obj || new Ground()).__init(this.bb!.__indirect(this.bb_pos + offset), this.bb!) : null;
}

static startAction(builder:flatbuffers.Builder) {
  builder.startObject(4);
}

static addWalk(builder:flatbuffers.Builder, walkOffset:flatbuffers.Offset) {
  builder.addFieldOffset(0, walkOffset, 0);
}

static addJump(builder:flatbuffers.Builder, jumpOffset:flatbuffers.Offset) {
  builder.addFieldOffset(1, jumpOffset, 0);
}

static addGaze(builder:flatbuffers.Builder, gazeOffset:flatbuffers.Offset) {
  builder.addFieldOffset(2, gazeOffset, 0);
}

static addGround(builder:flatbuffers.Builder, groundOffset:flatbuffers.Offset) {
  builder.addFieldOffset(3, groundOffset, 0);
}

static endAction(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static finishActionBuffer(builder:flatbuffers.Builder, offset:flatbuffers.Offset) {
  builder.finish(offset);
}

static finishSizePrefixedActionBuffer(builder:flatbuffers.Builder, offset:flatbuffers.Offset) {
  builder.finish(offset, undefined, true);
}

}
