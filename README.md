# Vertex Block Descent

## Introduction

This is an implementation of _vertex block descent_ (VBD) for elastic simulation.

## Preprocessing

This section is only relevant if you want to use your own meshes. To use the example meshes, just unzip
`assets/experiments/01.zip` and `assets/experiments/02.zip` in `assets/experiments`.

Preprocessing is done in Python using the `pymesh` package. The instructions below run `pymesh` inside its official Docker container image.

On Fedora, try `-v "$(pwd):/workspace:Z"` if you run into a permission error on the mounted `.py`
file. You may also need to run with `sudo` privileges.

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
    -e 01
```

## Usage

This project relies on C++17, OpenGL 4.6, Conan 2, and CMake. To install dependencies and build from the root directory, create a default [Conan profile](https://docs.conan.io/2/reference/config_files/profiles.html) and run the following commands. When using an alternative Conan profile, change the `--preset` argument as necessary.

On Windows:

```bash
conan install . --build=missing
cmake --preset conan-default
cmake --build --preset conan-release
./build/Release/vbd-elasto-sim.exe 01  # for scenario 1 (cow)
./build/Release/vbd-elasto-sim.exe 02  # for scenario 2 (bunny)
```

On Linux/MacOS:

```bash
conan install . --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
./build/Release/vbd-elasto-sim 01  # for scenario 1 (cow)
./build/Release/vbd-elasto-sim 02  # for scenario 2 (bunny)
```

Use `W`, `A`, `S`, `D`, `Space`, and `Shift` to move around. Use `P` to play/pause the simulation.
Use `R` to reset the simulation. Use `Esc` to exit.

## References
Anka He Chen et al. “Vertex Block Descent”. In: ACM Transactions on Graphics (TOG) 116 (2024).

Keenan Crane, Ulrich Pinkall, and Peter Schr¨oder. “Robust fairing via conformal curvature flow”. In: ACM Transactions on Graphics (TOG) 32.4 (2013), pp. 1–10

Greg Turk and Marc Levoy. Stanford Bunny. 3D computer graphics test model developed by Greg Turk and Marc Levoy at Stanford University (69,451 triangles). Aug. 2000. url: https://graphics.stanford.edu/data/3Dscanrep/.
