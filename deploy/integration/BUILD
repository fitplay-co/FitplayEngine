load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "hello-time",
    srcs = ["hello-time.cc"],
    hdrs = ["hello-time.h"],
    copts = ["--std=c++17"],
    visibility = ["//visibility:public"],
    deps = [
        ":protocol",
        "//integration/third_party:glm",
        "//midware:midware",
        # "@com_gitlab_libeigen_eigen//:eigen",
        "@ceres_solver//:ceres",
    ],
    includes = ["protocol"]
)

cc_library(
    name = "protocol",
    hdrs = glob(["protocol/**/*.hpp", "protocol/**/*.h"]),
    visibility = ["//visibility:public"],
    copts = ["--std=c++17"],
    deps = [
        "@ceres_solver//:ceres",
        "//integration/third_party:flatbuffers",
    ],
    includes = ["protocol"]
)