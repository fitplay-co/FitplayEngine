// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_TEST_TEST_H_
#define FLATBUFFERS_GENERATED_TEST_TEST_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 2 &&
              FLATBUFFERS_VERSION_MINOR == 0 &&
              FLATBUFFERS_VERSION_REVISION == 6,
             "Non-compatible flatbuffers version included");

namespace Test {

struct TestC;
struct TestCBuilder;

struct TestC FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef TestCBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4
  };
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  flatbuffers::String *mutable_name() {
    return GetPointer<flatbuffers::String *>(VT_NAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
};

struct TestCBuilder {
  typedef TestC Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(TestC::VT_NAME, name);
  }
  explicit TestCBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<TestC> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<TestC>(end);
    return o;
  }
};

inline flatbuffers::Offset<TestC> CreateTestC(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  TestCBuilder builder_(_fbb);
  builder_.add_name(name);
  return builder_.Finish();
}

inline flatbuffers::Offset<TestC> CreateTestCDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return Test::CreateTestC(
      _fbb,
      name__);
}

inline const Test::TestC *GetTestC(const void *buf) {
  return flatbuffers::GetRoot<Test::TestC>(buf);
}

inline const Test::TestC *GetSizePrefixedTestC(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<Test::TestC>(buf);
}

inline TestC *GetMutableTestC(void *buf) {
  return flatbuffers::GetMutableRoot<TestC>(buf);
}

inline Test::TestC *GetMutableSizePrefixedTestC(void *buf) {
  return flatbuffers::GetMutableSizePrefixedRoot<Test::TestC>(buf);
}

inline bool VerifyTestCBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<Test::TestC>(nullptr);
}

inline bool VerifySizePrefixedTestCBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<Test::TestC>(nullptr);
}

inline void FinishTestCBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Test::TestC> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedTestCBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<Test::TestC> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace Test

#endif  // FLATBUFFERS_GENERATED_TEST_TEST_H_
