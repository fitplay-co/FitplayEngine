export BUILD_PLATFORM="iOS"
export SDKROOT="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
export XCODE_TOOLCHAINS="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain"
export CPPFLAGS="-I$SDKROOT/usr/include/"
export CFLAGS="$CPPFLAGS -arch arm64 -isysroot $SDKROOT"
export LD="$XCODE_TOOLCHAINS/usr/bin/ld"
export AR="$XCODE_TOOLCHAINS/usr/bin/ar"
export CXX="$XCODE_TOOLCHAINS/usr/bin/clang++ -arch arm64"
export CC="$XCODE_TOOLCHAINS/usr/bin/clang -arch arm64"

make clean
make

export BUILD_DIR="../../FitplayServer/FitplayServer/midware"

mkdir -p $BUILD_DIR/lib
mkdir -p $BUILD_DIR/include

cp lib/iOS/fitplaymidware.a $BUILD_DIR/lib
cp ./*.h $BUILD_DIR/include
cp ../common/third_party/json.hpp $BUILD_DIR/include
cp ../common/midware/FPLandmarks.h $BUILD_DIR/include
