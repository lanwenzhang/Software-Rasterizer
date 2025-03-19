# Software-Rasterizer

## Screenshot
![01](https://github.com/user-attachments/assets/9ff5c552-1baf-4f1f-9628-05077712ddaa)

## Features
### Model loading
* Model(.fbx, .obj)
* Texture diffuse map

### Texture mapping
* Texture wrapping: repeat and mirror
* Texture filering: nearest and bilinear

### Clipping and Culling
* Viewport Clipping(Sutherland-Hodgman)
* Back-face Culling

### Lighting
* Lambertian Model

## Build
```
git clone https://github.com/lanwenzhang/Software-Rasterizer
cd Software-Rasterizer
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Dependencies
* [stb](https://github.com/nothings/stb)
* [Assimp](https://github.com/assimp/assimp) 

## References
* [GAMES 101, Introduction to Computer Graphics](https://sites.cs.ucsb.edu/~lingqi/teaching/games202.html)
* [Learn OpenGL Tutorial](https://learnopengl.com)
  
