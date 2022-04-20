# activate emsdk
./emsdk activate latest
source ./emsdk_env.sh

<!-- emcc -o hello.html demo.cc -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']" -s EXPORTED_FUNCTIONS="['_fun1']"
emcc fun1.cpp -o fun1.js -s EXPORTED_FUNCTIONS="['_int_sqrt']"
emcc -lembind -o develop.js develop.cc -->

# emscripten build command
emcc -lembind -o develop_class.js develop_class.cc -s "EXTRA_EXPORTED_RUNTIME_METHODS=['addOnPostRun']"

# cmake command
mkdir build
mkdir dist
cd build
emcmake cmake ..
emmake make