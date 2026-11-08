# titirenderer

A software rasterizer written from scratch in C++23. The CPU computes every pixel; SDL3 only puts the finished buffer in a window.

## What it does
- Loads Wavefront `.obj` meshes (vertices and triangle faces)
- Transforms vertices with a look-at camera, a perspective projection and a viewport transform, using its own small math library (vectors, 3×3 and 4×4 matrices; conventions are documented at the top of `src/Math.h`)
- Clips triangles against the near plane in clip space, before the perspective divide
- Fills triangles with barycentric coordinates and resolves visibility with a z-buffer
- Shades pixels by depth (grayscale)

## Build and run
Needs g++ with C++23, `make` and SDL3 (Linux through `pkg-config`, macOS through Homebrew, Windows through MSYS2 UCRT64).

```
make run
```

The model path is hardcoded in `src/main.cpp`: put a mesh at `assets/diablo3_pose.obj` (the `assets/` folder is not committed) and run from the repository root.

## Credits
The test model `diablo3_pose.obj` comes from [ssloy/tinyrenderer](https://github.com/ssloy/tinyrenderer).

## License
MIT
