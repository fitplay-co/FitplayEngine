load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "midware",
    hdrs = glob(["**/*.hpp", "**/*.h"]),
    visibility = ["//visibility:public"],
    copts = ["--std=c++17"],
    deps = [
        "//integration/third_party:flatbuffers",
        "//integration:protocol",
    ],
    includes = ["."]
)