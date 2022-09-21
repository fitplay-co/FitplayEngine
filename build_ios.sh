bazel build --config=ios ios:FitplayPoseTrackerFramework
sudo rm -r build/
mkdir -p build/
cp bazel-out/**/bin/ios/FitplayPoseTrackerFramework.zip build/
cd build
unzip *.zip
