# activate emsdk
./emsdk activate latest
source ./emsdk_env.sh

# emscripten build command
emcc -lembind -o develop_class.js develop_class.cc -s "EXTRA_EXPORTED_RUNTIME_METHODS=['addOnPostRun']"

# cmake command
mkdir build
cd build
emcmake cmake ..
emmake make