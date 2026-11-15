# tonyMesh

A C++20 library for processing triangle meshes, plus an OpenGL viewer to inspect them.

## Build

Needs CMake 3.25+, Ninja, a C++20 compiler and Python 3; all other dependencies are downloaded automatically on the first configure.

```sh
cmake --preset release
cmake --build --preset release
```

## Run

From the repository root, so the mesh is found in `data/`:

```sh
./build/release/src/app/tonyMesh_app --open honda.off
```

## Test

```sh
ctest --preset debug   # after building the debug preset
```
