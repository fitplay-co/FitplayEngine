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
    deps = [
        "//integration:hello-time",
        "//integration:protocol",
    ]
)


ios_framework(
    name = "FitplayPoseTracker",
    bundle_id = "com.fitplay.FitplayPoseTracker",
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
