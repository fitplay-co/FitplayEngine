// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_ACTIONDATA_ACTIONDATA_H_
#define FLATBUFFERS_GENERATED_ACTIONDATA_ACTIONDATA_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 2 &&
              FLATBUFFERS_VERSION_MINOR == 0 &&
              FLATBUFFERS_VERSION_REVISION == 6,
             "Non-compatible flatbuffers version included");

namespace actionData {

struct Walk;
struct WalkBuilder;

struct Jump;
struct JumpBuilder;

struct Gaze;
struct GazeBuilder;

struct Ground;
struct GroundBuilder;

struct Joint;
struct JointBuilder;

struct Action;
struct ActionBuilder;

struct Walk FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef WalkBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_LEGUP = 4,
    VT_FREQUENCY = 6,
    VT_STRENGTH = 8
  };
  float legUp() const {
    return GetField<float>(VT_LEGUP, 0.0f);
  }
  float frequency() const {
    return GetField<float>(VT_FREQUENCY, 0.0f);
  }
  float strength() const {
    return GetField<float>(VT_STRENGTH, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_LEGUP, 4) &&
           VerifyField<float>(verifier, VT_FREQUENCY, 4) &&
           VerifyField<float>(verifier, VT_STRENGTH, 4) &&
           verifier.EndTable();
  }
};

struct WalkBuilder {
  typedef Walk Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_legUp(float legUp) {
    fbb_.AddElement<float>(Walk::VT_LEGUP, legUp, 0.0f);
  }
  void add_frequency(float frequency) {
    fbb_.AddElement<float>(Walk::VT_FREQUENCY, frequency, 0.0f);
  }
  void add_strength(float strength) {
    fbb_.AddElement<float>(Walk::VT_STRENGTH, strength, 0.0f);
  }
  explicit WalkBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Walk> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Walk>(end);
    return o;
  }
};

inline flatbuffers::Offset<Walk> CreateWalk(
    flatbuffers::FlatBufferBuilder &_fbb,
    float legUp = 0.0f,
    float frequency = 0.0f,
    float strength = 0.0f) {
  WalkBuilder builder_(_fbb);
  builder_.add_strength(strength);
  builder_.add_frequency(frequency);
  builder_.add_legUp(legUp);
  return builder_.Finish();
}

struct Jump FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef JumpBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_UP = 4,
    VT_STRENGTH = 6
  };
  float up() const {
    return GetField<float>(VT_UP, 0.0f);
  }
  float strength() const {
    return GetField<float>(VT_STRENGTH, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_UP, 4) &&
           VerifyField<float>(verifier, VT_STRENGTH, 4) &&
           verifier.EndTable();
  }
};

struct JumpBuilder {
  typedef Jump Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_up(float up) {
    fbb_.AddElement<float>(Jump::VT_UP, up, 0.0f);
  }
  void add_strength(float strength) {
    fbb_.AddElement<float>(Jump::VT_STRENGTH, strength, 0.0f);
  }
  explicit JumpBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Jump> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Jump>(end);
    return o;
  }
};

inline flatbuffers::Offset<Jump> CreateJump(
    flatbuffers::FlatBufferBuilder &_fbb,
    float up = 0.0f,
    float strength = 0.0f) {
  JumpBuilder builder_(_fbb);
  builder_.add_strength(strength);
  builder_.add_up(up);
  return builder_.Finish();
}

struct Gaze FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef GazeBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_Z, 4) &&
           verifier.EndTable();
  }
};

struct GazeBuilder {
  typedef Gaze Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Gaze::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Gaze::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(Gaze::VT_Z, z, 0.0f);
  }
  explicit GazeBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Gaze> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Gaze>(end);
    return o;
  }
};

inline flatbuffers::Offset<Gaze> CreateGaze(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f) {
  GazeBuilder builder_(_fbb);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

struct Ground FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef GroundBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8,
    VT_LEGLENGTH = 10,
    VT_TRACING = 12
  };
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  float legLength() const {
    return GetField<float>(VT_LEGLENGTH, 0.0f);
  }
  float tracing() const {
    return GetField<float>(VT_TRACING, 0.0f);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_Z, 4) &&
           VerifyField<float>(verifier, VT_LEGLENGTH, 4) &&
           VerifyField<float>(verifier, VT_TRACING, 4) &&
           verifier.EndTable();
  }
};

struct GroundBuilder {
  typedef Ground Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Ground::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Ground::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(Ground::VT_Z, z, 0.0f);
  }
  void add_legLength(float legLength) {
    fbb_.AddElement<float>(Ground::VT_LEGLENGTH, legLength, 0.0f);
  }
  void add_tracing(float tracing) {
    fbb_.AddElement<float>(Ground::VT_TRACING, tracing, 0.0f);
  }
  explicit GroundBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Ground> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Ground>(end);
    return o;
  }
};

inline flatbuffers::Offset<Ground> CreateGround(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f,
    float legLength = 0.0f,
    float tracing = 0.0f) {
  GroundBuilder builder_(_fbb);
  builder_.add_tracing(tracing);
  builder_.add_legLength(legLength);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

struct Joint FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef JointBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_W = 4,
    VT_X = 6,
    VT_Y = 8,
    VT_Z = 10,
    VT_NAME = 12
  };
  float w() const {
    return GetField<float>(VT_W, 0.0f);
  }
  float x() const {
    return GetField<float>(VT_X, 0.0f);
  }
  float y() const {
    return GetField<float>(VT_Y, 0.0f);
  }
  float z() const {
    return GetField<float>(VT_Z, 0.0f);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_W, 4) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_Z, 4) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
};

struct JointBuilder {
  typedef Joint Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_w(float w) {
    fbb_.AddElement<float>(Joint::VT_W, w, 0.0f);
  }
  void add_x(float x) {
    fbb_.AddElement<float>(Joint::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Joint::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(Joint::VT_Z, z, 0.0f);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Joint::VT_NAME, name);
  }
  explicit JointBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Joint> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Joint>(end);
    return o;
  }
};

inline flatbuffers::Offset<Joint> CreateJoint(
    flatbuffers::FlatBufferBuilder &_fbb,
    float w = 0.0f,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f,
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  JointBuilder builder_(_fbb);
  builder_.add_name(name);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  builder_.add_w(w);
  return builder_.Finish();
}

inline flatbuffers::Offset<Joint> CreateJointDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    float w = 0.0f,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return actionData::CreateJoint(
      _fbb,
      w,
      x,
      y,
      z,
      name__);
}

struct Action FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ActionBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_WALK = 4,
    VT_JUMP = 6,
    VT_GAZE = 8,
    VT_GROUND = 10,
    VT_JOINTS = 12
  };
  const actionData::Walk *walk() const {
    return GetPointer<const actionData::Walk *>(VT_WALK);
  }
  const actionData::Jump *jump() const {
    return GetPointer<const actionData::Jump *>(VT_JUMP);
  }
  const actionData::Gaze *gaze() const {
    return GetPointer<const actionData::Gaze *>(VT_GAZE);
  }
  const actionData::Ground *ground() const {
    return GetPointer<const actionData::Ground *>(VT_GROUND);
  }
  const flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>> *joints() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>> *>(VT_JOINTS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_WALK) &&
           verifier.VerifyTable(walk()) &&
           VerifyOffset(verifier, VT_JUMP) &&
           verifier.VerifyTable(jump()) &&
           VerifyOffset(verifier, VT_GAZE) &&
           verifier.VerifyTable(gaze()) &&
           VerifyOffset(verifier, VT_GROUND) &&
           verifier.VerifyTable(ground()) &&
           VerifyOffset(verifier, VT_JOINTS) &&
           verifier.VerifyVector(joints()) &&
           verifier.VerifyVectorOfTables(joints()) &&
           verifier.EndTable();
  }
};

struct ActionBuilder {
  typedef Action Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_walk(flatbuffers::Offset<actionData::Walk> walk) {
    fbb_.AddOffset(Action::VT_WALK, walk);
  }
  void add_jump(flatbuffers::Offset<actionData::Jump> jump) {
    fbb_.AddOffset(Action::VT_JUMP, jump);
  }
  void add_gaze(flatbuffers::Offset<actionData::Gaze> gaze) {
    fbb_.AddOffset(Action::VT_GAZE, gaze);
  }
  void add_ground(flatbuffers::Offset<actionData::Ground> ground) {
    fbb_.AddOffset(Action::VT_GROUND, ground);
  }
  void add_joints(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> joints) {
    fbb_.AddOffset(Action::VT_JOINTS, joints);
  }
  explicit ActionBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Action> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Action>(end);
    return o;
  }
};

inline flatbuffers::Offset<Action> CreateAction(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<actionData::Walk> walk = 0,
    flatbuffers::Offset<actionData::Jump> jump = 0,
    flatbuffers::Offset<actionData::Gaze> gaze = 0,
    flatbuffers::Offset<actionData::Ground> ground = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<actionData::Joint>>> joints = 0) {
  ActionBuilder builder_(_fbb);
  builder_.add_joints(joints);
  builder_.add_ground(ground);
  builder_.add_gaze(gaze);
  builder_.add_jump(jump);
  builder_.add_walk(walk);
  return builder_.Finish();
}

inline flatbuffers::Offset<Action> CreateActionDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<actionData::Walk> walk = 0,
    flatbuffers::Offset<actionData::Jump> jump = 0,
    flatbuffers::Offset<actionData::Gaze> gaze = 0,
    flatbuffers::Offset<actionData::Ground> ground = 0,
    const std::vector<flatbuffers::Offset<actionData::Joint>> *joints = nullptr) {
  auto joints__ = joints ? _fbb.CreateVector<flatbuffers::Offset<actionData::Joint>>(*joints) : 0;
  return actionData::CreateAction(
      _fbb,
      walk,
      jump,
      gaze,
      ground,
      joints__);
}

inline const actionData::Action *GetAction(const void *buf) {
  return flatbuffers::GetRoot<actionData::Action>(buf);
}

inline const actionData::Action *GetSizePrefixedAction(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<actionData::Action>(buf);
}

inline bool VerifyActionBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<actionData::Action>(nullptr);
}

inline bool VerifySizePrefixedActionBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<actionData::Action>(nullptr);
}

inline void FinishActionBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<actionData::Action> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedActionBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<actionData::Action> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace actionData

#endif  // FLATBUFFERS_GENERATED_ACTIONDATA_ACTIONDATA_H_
