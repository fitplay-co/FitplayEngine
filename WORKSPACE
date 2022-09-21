load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "build_bazel_rules_apple",
    sha256 = "0052d452af7742c8f3a4e0929763388a66403de363775db7e90adecb2ba4944b",
    url = "https://github.com/bazelbuild/rules_apple/releases/download/0.31.3/rules_apple.0.31.3.tar.gz",
)

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()

load(
    "@build_bazel_rules_swift//swift:repositories.bzl",
    "swift_rules_dependencies",
)

swift_rules_dependencies()

load(
    "@build_bazel_apple_support//lib:repositories.bzl",
    "apple_support_dependencies",
)

apple_support_dependencies()

http_archive(
    name = "io_bazel_rules_appengine",
    strip_prefix = "rules_appengine-8099725bab2fa01ccd385fac2f78278ea8145a7a",
    # TODO: update to a release version that contains 339f6aba67fcedb7268cf54d1163cf7704a277ca.
    # This commit fixes the Maven artifact URLs to use "https" instead of "http".
    # We don't specify sha256, because the sha256 of GitHub-served non-release archives isn't
    # stable.
    urls = ["https://github.com/bazelbuild/rules_appengine/archive/8099725bab2fa01ccd385fac2f78278ea8145a7a.tar.gz"],
)

load(
    "@io_bazel_rules_appengine//appengine:java_appengine.bzl",
    "java_appengine_repositories",
)

java_appengine_repositories()

android_sdk_repository(name = "androidsdk")

http_archive(
    name = "rules_android",
    sha256 = "cd06d15dd8bb59926e4d65f9003bfc20f9da4b2519985c27e190cddc8b7a7806",
    strip_prefix = "rules_android-0.1.1",
    urls = ["https://github.com/bazelbuild/rules_android/archive/v0.1.1.zip"],
)

# emsdk 3.1.13
http_archive(
    name = "emsdk",
    strip_prefix = "emsdk-311acff345fd71dcfe5f350653cec466ee7e3fbc/bazel",
    url = "https://github.com/emscripten-core/emsdk/archive/311acff345fd71dcfe5f350653cec466ee7e3fbc.tar.gz",
)

load("@emsdk//:deps.bzl", emsdk_deps = "deps")
emsdk_deps()

load("@emsdk//:emscripten_deps.bzl", emsdk_emscripten_deps = "emscripten_deps")
emsdk_emscripten_deps()

# gtest 

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip"],
  strip_prefix = "googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
)

# ceres solver 
http_archive(
    name = "ceres_solver",
    url = "https://github.com/ceres-solver/ceres-solver/archive/123fba61cf2611a3c8bddc9d91416db26b10b558.zip",
    patches = [
        "@//common/third_party:ceres_solver_compatibility_fixes.diff"
    ],
    patch_args = [
        "-p1",
    ],
    strip_prefix = "ceres-solver-123fba61cf2611a3c8bddc9d91416db26b10b558",
    sha256 = "8b7b16ceb363420e0fd499576daf73fa338adb0b1449f58bea7862766baa1ac7"
)

# gflags needed by glog
http_archive(
    name = "com_github_gflags_gflags",
    strip_prefix = "gflags-2.2.2",
    sha256 = "19713a36c9f32b33df59d1c79b4958434cb005b5b47dc5400a7a4b078111d9b5",
    url = "https://github.com/gflags/gflags/archive/v2.2.2.zip",
)

http_archive(
    name = "com_github_glog_glog",
    strip_prefix = "glog-0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
    sha256 = "58c9b3b6aaa4dd8b836c0fd8f65d0f941441fb95e27212c5eeb9979cfd3592ab",
    urls = [
        "https://github.com/google/glog/archive/0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6.zip",
    ],
)

http_archive(
    name = "com_github_glog_glog_no_gflags",
    strip_prefix = "glog-0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
    sha256 = "58c9b3b6aaa4dd8b836c0fd8f65d0f941441fb95e27212c5eeb9979cfd3592ab",
    build_file = "@//third_party:glog_no_gflags.BUILD",
    urls = [
        "https://github.com/google/glog/archive/0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6.zip",
    ],
    patch_args = [
        "-p1",
    ],
)

# External dependency: Eigen; has no Bazel build.
http_archive(
    name = "eigen_archive",
    sha256 = "0215c6593c4ee9f1f7f28238c4e8995584ebf3b556e9dbf933d84feb98d5b9ef",
    strip_prefix = "eigen-3.3.8",
    urls = [
        "https://gitlab.com/libeigen/eigen/-/archive/3.3.8/eigen-3.3.8.tar.bz2",
    ],
    build_file_content =
"""
# TODO(keir): Replace this with a better version, like from TensorFlow.
# See https://github.com/ceres-solver/ceres-solver/issues/337.
cc_library(
    name = 'eigen',
    srcs = [],
    includes = ['.'],
    hdrs = glob(['Eigen/**']),
    visibility = ['//visibility:public'],
)
"""
)

# External dependency: Google Benchmark; has no Bazel build.
http_archive(
    name = "com_github_google_benchmark",
    urls = ["https://github.com/google/benchmark/archive/56f52ee228783547f544d9ac4a533574b9010e3f.zip"],
    sha256 = "8c1c6e90cd320b07504fabb86400f390faff2e599183ebd9396908817968ae79",
    strip_prefix = "benchmark-56f52ee228783547f544d9ac4a533574b9010e3f",
    build_file_content =
"""
cc_library(
    name = "benchmark",
    srcs = glob([
        "src/*.h",
        "src/*.cc",
    ]),
    hdrs = glob(["include/benchmark/*.h"]),
    copts = [
        "-DHAVE_STD_REGEX",
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
)
"""
)
