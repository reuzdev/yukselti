# Yükselti
Yükselti is a [Voxel Space](https://en.wikipedia.org/wiki/Voxel_Space) implementation -a 2.5D software rendering technique that uses color and height maps to render pseudo-3D terrain- written in C using [Raylib](https://www.raylib.com/).

<p align="left">
  <img src="https://img.shields.io/badge/C-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white" />
  <img src="https://img.shields.io/badge/Raylib-FFFFFF?style=for-the-badge&logo=raylib&logoColor=black" />
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white" />
</p>

![Screenshot](./resources/screenshot_no_ui.png)
![Screenshot](./resources/screenshot_with_ui.png)

## Acknowledgements
The idea and the provided map files are taken from [this](https://github.com/s-macke/VoxelSpace/) repo by s-macke.

## Building and Development

### Requirements
- CMake >= 3.24
- C compiler
- Raylib 5.5 (Automatically downloaded by CMake if not found)

### Building
> If you don't want to build in Release configuration omit the related flags:
```console
git clone https://github.com/reuzdev/yukselti.git
cd yukselti
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config=Release
```
Note that if you don't have Raylib 5.5 installed on your system CMake will automatically download and compile it for you.
This may take a while depending on your internet connection and system.

## License
[MIT](https://choosealicense.com/licenses/mit/)
