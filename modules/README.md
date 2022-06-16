# activate emsdk
./emsdk activate latest
source ./emsdk_env.sh

# cmake command
rm -rf build
mkdir build
cd build
emcmake cmake ..
emmake make
cd ..