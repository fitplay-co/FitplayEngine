load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "hello-time",
    srcs = ["hello-time.cc"],
    hdrs = ["hello-time.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":protocol",
        "//integration/third_party:glm",
        "//midware:midware"
    ],
    copts = ["--std=c++17"],
    includes = ["protocol"]
)

cc_library(
    name = "protocol",
    hdrs = glob(["protocol/**/*.hpp", "protocol/**/*.h"]),
    visibility = ["//visibility:public"],
    copts = ["--std=c++17"],
    deps = [
        "//integration/third_party:flatbuffers",
    ],
    includes = ["protocol"]
)