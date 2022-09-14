// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_FEATURECONFIG_FEATURECONFIGS_H_
#define FLATBUFFERS_GENERATED_FEATURECONFIG_FEATURECONFIGS_H_

#include "flatbuffers/flatbuffers.h"

namespace FeatureConfigs {

struct FeatureConfigList;
struct FeatureConfigListBuilder;

struct FeatureConfig;
struct FeatureConfigBuilder;

struct FeatureConfigList FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef FeatureConfigListBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_CONFIGS = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<FeatureConfigs::FeatureConfig>> *configs() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<FeatureConfigs::FeatureConfig>> *>(VT_CONFIGS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_CONFIGS) &&
           verifier.VerifyVector(configs()) &&
           verifier.VerifyVectorOfTables(configs()) &&
           verifier.EndTable();
  }
};

struct FeatureConfigListBuilder {
  typedef FeatureConfigList Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_configs(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FeatureConfigs::FeatureConfig>>> configs) {
    fbb_.AddOffset(FeatureConfigList::VT_CONFIGS, configs);
  }
  explicit FeatureConfigListBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<FeatureConfigList> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<FeatureConfigList>(end);
    return o;
  }
};

inline flatbuffers::Offset<FeatureConfigList> CreateFeatureConfigList(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<FeatureConfigs::FeatureConfig>>> configs = 0) {
  FeatureConfigListBuilder builder_(_fbb);
  builder_.add_configs(configs);
  return builder_.Finish();
}

inline flatbuffers::Offset<FeatureConfigList> CreateFeatureConfigListDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<FeatureConfigs::FeatureConfig>> *configs = nullptr) {
  auto configs__ = configs ? _fbb.CreateVector<flatbuffers::Offset<FeatureConfigs::FeatureConfig>>(*configs) : 0;
  return FeatureConfigs::CreateFeatureConfigList(
      _fbb,
      configs__);
}

struct FeatureConfig FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef FeatureConfigBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_FEATURE_ID = 4,
    VT_ENABLE = 6,
    VT_ACTION = 8,
    VT_TYPE = 10,
    VT_DATA = 12
  };
  const flatbuffers::String *feature_id() const {
    return GetPointer<const flatbuffers::String *>(VT_FEATURE_ID);
  }
  bool enable() const {
    return GetField<uint8_t>(VT_ENABLE, 0) != 0;
  }
  const flatbuffers::String *action() const {
    return GetPointer<const flatbuffers::String *>(VT_ACTION);
  }
  const flatbuffers::String *type() const {
    return GetPointer<const flatbuffers::String *>(VT_TYPE);
  }
  const flatbuffers::String *data() const {
    return GetPointer<const flatbuffers::String *>(VT_DATA);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_FEATURE_ID) &&
           verifier.VerifyString(feature_id()) &&
           VerifyField<uint8_t>(verifier, VT_ENABLE, 1) &&
           VerifyOffset(verifier, VT_ACTION) &&
           verifier.VerifyString(action()) &&
           VerifyOffset(verifier, VT_TYPE) &&
           verifier.VerifyString(type()) &&
           VerifyOffset(verifier, VT_DATA) &&
           verifier.VerifyString(data()) &&
           verifier.EndTable();
  }
};

struct FeatureConfigBuilder {
  typedef FeatureConfig Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_feature_id(flatbuffers::Offset<flatbuffers::String> feature_id) {
    fbb_.AddOffset(FeatureConfig::VT_FEATURE_ID, feature_id);
  }
  void add_enable(bool enable) {
    fbb_.AddElement<uint8_t>(FeatureConfig::VT_ENABLE, static_cast<uint8_t>(enable), 0);
  }
  void add_action(flatbuffers::Offset<flatbuffers::String> action) {
    fbb_.AddOffset(FeatureConfig::VT_ACTION, action);
  }
  void add_type(flatbuffers::Offset<flatbuffers::String> type) {
    fbb_.AddOffset(FeatureConfig::VT_TYPE, type);
  }
  void add_data(flatbuffers::Offset<flatbuffers::String> data) {
    fbb_.AddOffset(FeatureConfig::VT_DATA, data);
  }
  explicit FeatureConfigBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<FeatureConfig> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<FeatureConfig>(end);
    return o;
  }
};

inline flatbuffers::Offset<FeatureConfig> CreateFeatureConfig(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> feature_id = 0,
    bool enable = false,
    flatbuffers::Offset<flatbuffers::String> action = 0,
    flatbuffers::Offset<flatbuffers::String> type = 0,
    flatbuffers::Offset<flatbuffers::String> data = 0) {
  FeatureConfigBuilder builder_(_fbb);
  builder_.add_data(data);
  builder_.add_type(type);
  builder_.add_action(action);
  builder_.add_feature_id(feature_id);
  builder_.add_enable(enable);
  return builder_.Finish();
}

inline flatbuffers::Offset<FeatureConfig> CreateFeatureConfigDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *feature_id = nullptr,
    bool enable = false,
    const char *action = nullptr,
    const char *type = nullptr,
    const char *data = nullptr) {
  auto feature_id__ = feature_id ? _fbb.CreateString(feature_id) : 0;
  auto action__ = action ? _fbb.CreateString(action) : 0;
  auto type__ = type ? _fbb.CreateString(type) : 0;
  auto data__ = data ? _fbb.CreateString(data) : 0;
  return FeatureConfigs::CreateFeatureConfig(
      _fbb,
      feature_id__,
      enable,
      action__,
      type__,
      data__);
}

inline const FeatureConfigs::FeatureConfigList *GetFeatureConfigList(const void *buf) {
  return flatbuffers::GetRoot<FeatureConfigs::FeatureConfigList>(buf);
}

inline const FeatureConfigs::FeatureConfigList *GetSizePrefixedFeatureConfigList(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<FeatureConfigs::FeatureConfigList>(buf);
}

inline bool VerifyFeatureConfigListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FeatureConfigs::FeatureConfigList>(nullptr);
}

inline bool VerifySizePrefixedFeatureConfigListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FeatureConfigs::FeatureConfigList>(nullptr);
}

inline void FinishFeatureConfigListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FeatureConfigs::FeatureConfigList> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedFeatureConfigListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FeatureConfigs::FeatureConfigList> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace FeatureConfigs

#endif  // FLATBUFFERS_GENERATED_FEATURECONFIG_FEATURECONFIGS_H_
