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