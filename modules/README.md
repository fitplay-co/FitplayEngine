# activate emsdk
./emsdk activate latest
source ./emsdk_env.sh

# cmake command
cd ..
cd modules
rm -rf build
mkdir build
cd build
emcmake cmake ..
emmake make
cd ../..
cd app
npm start