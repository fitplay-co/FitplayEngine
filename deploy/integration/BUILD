load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "hello-time",
    srcs = ["hello-time.cc"],
    hdrs = ["hello-time.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":protocol"
    ]
)

cc_library(
    name = "protocol",
    hdrs = glob(["protocol/**/*.hpp", "protocol/**/*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        "//integration/third_party:flatbuffers",
        "//integration/third_party:glm",
    ]
)