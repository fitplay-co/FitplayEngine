rm -rf wasm/app/wasm_modules/*
bazel build --config=wasm //wasm:bridge-class-wasm
cp -f bazel-bin/wasm/bridge-class-wasm/* wasm/app/wasm_modules
