load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "glm",
    hdrs = glob(["glm/**/*.hpp", "glm/**/*.h"]),
    textual_hdrs = glob(["glm/**/*.inl"]),
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "json",
    hdrs = glob(["json.hpp"]),
    includes = ["."],
    visibility = ["//visibility:public"],
)
