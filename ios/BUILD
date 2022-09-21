load("@rules_cc//cc:defs.bzl", "objc_library")
load("@build_bazel_rules_apple//apple:ios.bzl", "ios_framework")

objc_library(
    name = "native-lib-oc",
    hdrs = [
        "FitplayPoseTracker.h",
    ],
    srcs = [
        "FitplayPoseTracker.mm",
    ],
    copts = ["--std=c++17"],
    deps = [
        "//common:hello-time",
        "//common:protocol",
    ]
)


ios_framework(
    name = "FitplayPoseTrackerFramework",
    bundle_id = "com.fitplay.FitplayPoseTrackerFramework",
    families = [
        "iphone",
        "ipad",
    ],
    hdrs = [
        "FitplayPoseTracker.h",
    ],
    infoplists = [":stub.plist"],
    minimum_os_version = "8.0",
    visibility = ["//visibility:public"],
    deps = [":native-lib-oc"],
)
