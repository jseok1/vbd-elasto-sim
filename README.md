# Vertex Block Descent

## Introduction



## Preprocessing

This section is only relevant if you want to use your own meshes.

### Tetrahedralization

```bash
docker run --rm -it \
  -v "$(pwd):/workspace" \
  pymesh/pymesh \
  python /workspace/tools/tetrahedralize.py \
    -i /workspace/assets/models/spot/spot_triangulated.obj \
    -o /workspace/assets/models/spot/spot_tetrahedralized.msh
```

### Parametrization

```bash
docker run --rm -it \
  -v "$(pwd):/workspace" \
  pymesh/pymesh \
  python /workspace/tools/parametrize.py \
    -i /workspace/assets/models/spot/spot_tetrahedralized.msh
```

On Fedora, try `-v "$(pwd):/workspace:Z"` if you run into a permission error on the mounted `.py`
file.

## Usage

This project relies on OpenGL 4.6, the Conan C/C++ package manager, and CMake build system. To install dependencies and build from the root directory, create a [Conan profile](https://docs.conan.io/2/reference/config_files/profiles.html) and run the following commands.

On Windows:

```bash
conan install . --build=missing --profile=release
cmake --preset conan-default
cmake --build --preset conan-release
./build/Release/sph-fluid-sim.exe
```

On Linux/MacOS:

```bash
conan install . --build=missing --profile=release
cmake --preset conan-release
cmake --build --preset conan-release
./build/Release/sph-fluid-sim
```

Use `W`, `A`, `S`, `D`, `Space`, and `Shift` to move around. Use `P` to play/pause the simulation.

## References

