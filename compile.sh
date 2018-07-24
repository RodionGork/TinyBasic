docker run --rm -v $(pwd):/src trzeci/emscripten emcc sqr.cpp -o sqr.js --closure 1 -s EXPORTED_FUNCTIONS='["_sqr"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s WASM=0
