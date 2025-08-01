# MarkdownParser

This is library for Markdown parsing, implemented based on C++ Qt6.

## Build

### Dependencies

- cmake: version **3.16** minimum
- Qt: **6.9.0 msvc2022\_64** is recommended (Other versions are untested but presumably available)

### Windows
Only x64 is tested on Windows.

    mkdir build && cd build
    cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release

### Linux
Not tested.

### Mac
Not tested.

### Additional build options
CMake options:

| Option              | Description                                                                                         | Default |
|---------------------|-----------------------------------------------------------------------------------------------------|---------|
| `BUILD_SHARED_LIBS` | ON: build a shared link library; OFF: build a static link library.                                  | `ON`    |
| `BUILD_SAMPLES`     | Build samples.                                                                                      | `ON`    |
| `BUILD_TESTS`       | Build tests.                                                                                        | `ON`    |
| `CMAKE_PREFIX_PATH` | Path to find dependencies. Must be set to the installation directory of the Qt kit you want to use. |         |

## About Tests

Using `QtTest` module. Please require `find_package` added in your `CMakeLists.txt`.

## About Samples

Please be sure to save sample `.md` file with `UTF-8`.