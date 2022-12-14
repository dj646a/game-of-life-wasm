# Game of Life in WebAssembly

This is a project to explore the targetting [Web Assembly](https://webassembly.org/) with C++ in a [freestanding](https://en.cppreference.com/w/cpp/freestanding) environment.

The fact that it's compiled in a free standing environment means that
we do not rely on a pre-existing Web Assembly runtime environments such as those provided by [Emscripten](https://emscripten.org/).

In theory this will enable us to distribute our application with a smaller
footprint.

# Quick start
#### Native build
```
clang++ -O3 -include ./source/base.hpp ./source/*.cpp -o game-of-life -lSDL2 -lGL -lGLEW -lstb -o game-of-life && ./game-of-life
```
#### Wasm build
```
Coming soon
```
