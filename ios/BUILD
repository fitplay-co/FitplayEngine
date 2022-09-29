load("@rules_cc//cc:defs.bzl", "objc_library")
load("@build_bazel_rules_apple//apple:ios.bzl", "ios_framework")

cc_library(
    name = "native-lib-cc",
    hdrs = [
        "MidwareBridge.h",
    ],
    srcs = [
        "MidwareBridge.cc",
    ],
    copts = ["--std=c++17"],
    deps = [
        "//common:common_lib",
    ]
)

ios_framework(
    name = "FitplayMidwareFramework",
    bundle_id = "com.fitplay.FitplayMidwareFramework",
    families = [
        "iphone",
        "ipad",
    ],
    hdrs = [
        "MidwareBridge.h",
    ],
    infoplists = [":stub.plist"],
    minimum_os_version = "8.0",
    visibility = ["//visibility:public"],
    deps = [":native-lib-cc"],
)
