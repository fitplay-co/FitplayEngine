// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

export class Walk {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
__init(i:number, bb:flatbuffers.ByteBuffer):Walk {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsWalk(bb:flatbuffers.ByteBuffer, obj?:Walk):Walk {
  return (obj || new Walk()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsWalk(bb:flatbuffers.ByteBuffer, obj?:Walk):Walk {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new Walk()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

legUp():number {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? this.bb!.readFloat32(this.bb_pos + offset) : 0.0;
}

frequency():number {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? this.bb!.readFloat32(this.bb_pos + offset) : 0.0;
}

strength():number {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? this.bb!.readFloat32(this.bb_pos + offset) : 0.0;
}

static startWalk(builder:flatbuffers.Builder) {
  builder.startObject(3);
}

static addLegUp(builder:flatbuffers.Builder, legUp:number) {
  builder.addFieldFloat32(0, legUp, 0.0);
}

static addFrequency(builder:flatbuffers.Builder, frequency:number) {
  builder.addFieldFloat32(1, frequency, 0.0);
}

static addStrength(builder:flatbuffers.Builder, strength:number) {
  builder.addFieldFloat32(2, strength, 0.0);
}

static endWalk(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static createWalk(builder:flatbuffers.Builder, legUp:number, frequency:number, strength:number):flatbuffers.Offset {
  Walk.startWalk(builder);
  Walk.addLegUp(builder, legUp);
  Walk.addFrequency(builder, frequency);
  Walk.addStrength(builder, strength);
  return Walk.endWalk(builder);
}
}
