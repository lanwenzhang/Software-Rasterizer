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
### 1. Clone the Repository
```sh
git clone https://github.com/lanwenzhang/Software-Rasterizer.git
cd Software-Rasterizer
```

### 2. Install Required Dependencies
#### **Windows**
- Install [CMake](https://cmake.org/download/)
- Install a C++ compiler (MinGW or Visual Studio)

#### Linux/macOS
Ensure CMake and a C++ compiler (`g++`/`clang`) are installed:
```sh
sudo apt install cmake g++    # Ubuntu/Debian
brew install cmake            # macOS
```

### 3. Create a Build Directory
```sh
mkdir build
cd build
```

### 4. Run CMake to Configure the Project

#### Windows (Visual Studio Generator)
```sh
cmake .. -G "Visual Studio 17 2022"
```

#### Windows (MinGW)
```sh
cmake .. -G "MinGW Makefiles"
```

#### Linux/macOS
```sh
cmake ..
```

### 5. Compile the Project
#### Windows (Visual Studio)
Open `Software-Rasterizer.sln` in Visual Studio and build.

#### Windows (MinGW)
```sh
mingw32-make
```

#### Linux/macOS
```sh
make
```

### 6. Run the Executable
```sh
./SoftwareRasterizer  # Linux/macOS
SoftwareRasterizer.exe  # Windows
```


## Dependencies
* [stb](https://github.com/nothings/stb)
* [Assimp](https://github.com/assimp/assimp) 

## References
* [GAMES 101, Introduction to Computer Graphics](https://sites.cs.ucsb.edu/~lingqi/teaching/games202.html)
* [Learn OpenGL Tutorial](https://learnopengl.com)
  
