cp -f protocol/cpp/*.h deploy/integration/protocol
rm -rf app/wasm_modules/*
cd deploy
bazel build --config=wasm //wasm:bridge-class-wasm
cd ..
cp -f deploy/bazel-bin/wasm/bridge-class-wasm/* app/wasm_modules