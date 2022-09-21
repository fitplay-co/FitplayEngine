cc_test(
  name = "gtest_hello",
  size = "small",
  srcs = ["gtest_hello.cc"],
  deps = ["@com_google_googletest//:gtest_main"],
)

cc_test(
  name = "gtest_ceres_hello",
  size = "small",
  srcs = ["gtest_ceres_hello.cc"],
  deps = [
    "@com_google_googletest//:gtest_main",
    "@ceres_solver//:ceres",
  ],
)

cc_test(
    name = "file_reading_hello",
    size = "small",
    srcs = ["file_reading_hello.cc"],
    deps = [
      "@com_google_googletest//:gtest_main",
    ],
    data = [
      "//test/data:testfile.csv"
    ]
)

cc_test(
    name = "old_tests",
    size = "small",
    srcs = [
      "old_tests.cc",
      "testUtils.hpp",
      "testGlm.hpp",
      "testDefaultTPose.hpp",
      "testDefaultStandPose.hpp",
      "testCapturedStandPose.hpp",
      "testFittingStandPose.hpp",
      "testUtilsMocLandmark.hpp",
      "testFittingRgbd.hpp",
      "testFittingStandPoseGravity.hpp"
    ],
    deps = [
      "@com_google_googletest//:gtest_main",
      "//midware:midware",
      "//integration/third_party:glm",
    ],
    data = [
      "//test/data:testfile.csv",
      "//test/data:test_data_full.csv",
      "//test/data:jump_data_full.csv",
      "//test/data:occlusion_data_full.csv",
      "//test/data:replay.csv",
      "//test/data:replay2.csv",
      "//test/data:rgbd_stand_pose_test.csv",
      "//test/data:test_data_heavy.csv",
    ]
)

