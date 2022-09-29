bazel build --config=ios ios:FitplayMidwareFramework
sudo rm -r build/
mkdir -p build/
cp bazel-out/**/bin/ios/FitplayMidwareFramework.zip build/
cd build
unzip *.zip
cd ..
cp common/third_party/*.hpp build/*.framework/Headers
cp -r build/*.framework ../FitplayServer/
