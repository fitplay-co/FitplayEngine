// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_IMUDATA_IMUDATA_H_
#define FLATBUFFERS_GENERATED_IMUDATA_IMUDATA_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 2 &&
              FLATBUFFERS_VERSION_MINOR == 0 &&
              FLATBUFFERS_VERSION_REVISION == 6,
             "Non-compatible flatbuffers version included");

namespace ImuData {

struct Vec3;

struct Imu;
struct ImuBuilder;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vec3 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Vec3()
      : x_(0),
        y_(0),
        z_(0) {
  }
  Vec3(float _x, float _y, float _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
  float z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Vec3, 12);

struct Imu FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef ImuBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_DEVICE_ID = 4,
    VT_SEQ = 6,
    VT_ACCELEROMETER = 8,
    VT_GYROSCOPE = 10,
    VT_MAGNETOMETER = 12
  };
  int16_t device_id() const {
    return GetField<int16_t>(VT_DEVICE_ID, 0);
  }
  int32_t seq() const {
    return GetField<int32_t>(VT_SEQ, 0);
  }
  const ImuData::Vec3 *accelerometer() const {
    return GetStruct<const ImuData::Vec3 *>(VT_ACCELEROMETER);
  }
  const ImuData::Vec3 *gyroscope() const {
    return GetStruct<const ImuData::Vec3 *>(VT_GYROSCOPE);
  }
  const ImuData::Vec3 *magnetometer() const {
    return GetStruct<const ImuData::Vec3 *>(VT_MAGNETOMETER);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int16_t>(verifier, VT_DEVICE_ID, 2) &&
           VerifyField<int32_t>(verifier, VT_SEQ, 4) &&
           VerifyField<ImuData::Vec3>(verifier, VT_ACCELEROMETER, 4) &&
           VerifyField<ImuData::Vec3>(verifier, VT_GYROSCOPE, 4) &&
           VerifyField<ImuData::Vec3>(verifier, VT_MAGNETOMETER, 4) &&
           verifier.EndTable();
  }
};

struct ImuBuilder {
  typedef Imu Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_device_id(int16_t device_id) {
    fbb_.AddElement<int16_t>(Imu::VT_DEVICE_ID, device_id, 0);
  }
  void add_seq(int32_t seq) {
    fbb_.AddElement<int32_t>(Imu::VT_SEQ, seq, 0);
  }
  void add_accelerometer(const ImuData::Vec3 *accelerometer) {
    fbb_.AddStruct(Imu::VT_ACCELEROMETER, accelerometer);
  }
  void add_gyroscope(const ImuData::Vec3 *gyroscope) {
    fbb_.AddStruct(Imu::VT_GYROSCOPE, gyroscope);
  }
  void add_magnetometer(const ImuData::Vec3 *magnetometer) {
    fbb_.AddStruct(Imu::VT_MAGNETOMETER, magnetometer);
  }
  explicit ImuBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Imu> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Imu>(end);
    return o;
  }
};

inline flatbuffers::Offset<Imu> CreateImu(
    flatbuffers::FlatBufferBuilder &_fbb,
    int16_t device_id = 0,
    int32_t seq = 0,
    const ImuData::Vec3 *accelerometer = nullptr,
    const ImuData::Vec3 *gyroscope = nullptr,
    const ImuData::Vec3 *magnetometer = nullptr) {
  ImuBuilder builder_(_fbb);
  builder_.add_magnetometer(magnetometer);
  builder_.add_gyroscope(gyroscope);
  builder_.add_accelerometer(accelerometer);
  builder_.add_seq(seq);
  builder_.add_device_id(device_id);
  return builder_.Finish();
}

inline const ImuData::Imu *GetImu(const void *buf) {
  return flatbuffers::GetRoot<ImuData::Imu>(buf);
}

inline const ImuData::Imu *GetSizePrefixedImu(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<ImuData::Imu>(buf);
}

inline bool VerifyImuBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<ImuData::Imu>(nullptr);
}

inline bool VerifySizePrefixedImuBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<ImuData::Imu>(nullptr);
}

inline void FinishImuBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<ImuData::Imu> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedImuBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<ImuData::Imu> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace ImuData

#endif  // FLATBUFFERS_GENERATED_IMUDATA_IMUDATA_H_
