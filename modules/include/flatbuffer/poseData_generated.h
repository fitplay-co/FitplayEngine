// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_POSEDATA_POSEDATA_H_
#define FLATBUFFERS_GENERATED_POSEDATA_POSEDATA_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 2 &&
              FLATBUFFERS_VERSION_MINOR == 0 &&
              FLATBUFFERS_VERSION_REVISION == 6,
             "Non-compatible flatbuffers version included");

namespace PoseData {

struct Pose;
struct PoseBuilder;

struct Point;
struct PointBuilder;

struct Pose FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PoseBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_KEYPOINTS = 4,
    VT_KEYPOINTS3D = 6
  };
  const flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>> *keypoints() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>> *>(VT_KEYPOINTS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>> *keypoints3D() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>> *>(VT_KEYPOINTS3D);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_KEYPOINTS) &&
           verifier.VerifyVector(keypoints()) &&
           verifier.VerifyVectorOfTables(keypoints()) &&
           VerifyOffset(verifier, VT_KEYPOINTS3D) &&
           verifier.VerifyVector(keypoints3D()) &&
           verifier.VerifyVectorOfTables(keypoints3D()) &&
           verifier.EndTable();
  }
};

struct PoseBuilder {
  typedef Pose Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_keypoints(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>>> keypoints) {
    fbb_.AddOffset(Pose::VT_KEYPOINTS, keypoints);
  }
  void add_keypoints3D(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>>> keypoints3D) {
    fbb_.AddOffset(Pose::VT_KEYPOINTS3D, keypoints3D);
  }
  explicit PoseBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Pose> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Pose>(end);
    return o;
  }
};

inline flatbuffers::Offset<Pose> CreatePose(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>>> keypoints = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<PoseData::Point>>> keypoints3D = 0) {
  PoseBuilder builder_(_fbb);
  builder_.add_keypoints3D(keypoints3D);
  builder_.add_keypoints(keypoints);
  return builder_.Finish();
}

inline flatbuffers::Offset<Pose> CreatePoseDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<PoseData::Point>> *keypoints = nullptr,
    const std::vector<flatbuffers::Offset<PoseData::Point>> *keypoints3D = nullptr) {
  auto keypoints__ = keypoints ? _fbb.CreateVector<flatbuffers::Offset<PoseData::Point>>(*keypoints) : 0;
  auto keypoints3D__ = keypoints3D ? _fbb.CreateVector<flatbuffers::Offset<PoseData::Point>>(*keypoints3D) : 0;
  return PoseData::CreatePose(
      _fbb,
      keypoints__,
      keypoints3D__);
}

struct Point FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PointBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_Z = 8,
    VT_SCORE = 10,
    VT_NAME = 12
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
  float score() const {
    return GetField<float>(VT_SCORE, 0.0f);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_X, 4) &&
           VerifyField<float>(verifier, VT_Y, 4) &&
           VerifyField<float>(verifier, VT_Z, 4) &&
           VerifyField<float>(verifier, VT_SCORE, 4) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
};

struct PointBuilder {
  typedef Point Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(float x) {
    fbb_.AddElement<float>(Point::VT_X, x, 0.0f);
  }
  void add_y(float y) {
    fbb_.AddElement<float>(Point::VT_Y, y, 0.0f);
  }
  void add_z(float z) {
    fbb_.AddElement<float>(Point::VT_Z, z, 0.0f);
  }
  void add_score(float score) {
    fbb_.AddElement<float>(Point::VT_SCORE, score, 0.0f);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Point::VT_NAME, name);
  }
  explicit PointBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Point> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Point>(end);
    return o;
  }
};

inline flatbuffers::Offset<Point> CreatePoint(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f,
    float score = 0.0f,
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  PointBuilder builder_(_fbb);
  builder_.add_name(name);
  builder_.add_score(score);
  builder_.add_z(z);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

inline flatbuffers::Offset<Point> CreatePointDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    float x = 0.0f,
    float y = 0.0f,
    float z = 0.0f,
    float score = 0.0f,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return PoseData::CreatePoint(
      _fbb,
      x,
      y,
      z,
      score,
      name__);
}

inline const PoseData::Pose *GetPose(const void *buf) {
  return flatbuffers::GetRoot<PoseData::Pose>(buf);
}

inline const PoseData::Pose *GetSizePrefixedPose(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<PoseData::Pose>(buf);
}

inline bool VerifyPoseBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<PoseData::Pose>(nullptr);
}

inline bool VerifySizePrefixedPoseBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<PoseData::Pose>(nullptr);
}

inline void FinishPoseBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<PoseData::Pose> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedPoseBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<PoseData::Pose> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace PoseData

#endif  // FLATBUFFERS_GENERATED_POSEDATA_POSEDATA_H_
