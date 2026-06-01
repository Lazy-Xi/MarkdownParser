# MarkdownParser

A Markdown-to-HTML library implemented in C++23 on top of Qt6 (Core). It converts a Markdown string into a complete, styled HTML document.

## Features

- Block elements: headings, paragraphs, ordered/unordered lists (nestable), block quotes (nestable), code blocks, tables, horizontal rules, and raw HTML.
- Inline elements: bold, italic, bold-italic, highlight, strikethrough, inline code, links, images, and escapes.
- Emits a full HTML document with an embedded, minified default stylesheet; the first `<h1>` becomes the document `<title>`.
- Customizable styling via `setStyleSheet`.

## Usage

Link against the `MarkdownParser` target and Qt6::Core, then call `toHtml`:

```cpp
#include "MarkdownParser.h"
#include <QString>

MarkdownParser parser;
// Optional: override the default stylesheet.
// parser.setStyleSheet("body { font-family: sans-serif; }");

const QString html = parser.toHtml("# Hello\n\nThis is **Markdown**.");
```

The returned string is a self-contained `<html>…</html>` document ready to write to a `.html` file.

## Build

### Dependencies

- **CMake 3.21+** — required by the `"version": 3` `CMakePresets.json`. The bare `CMakeLists.txt` itself only needs 3.19.
- **Qt 6 (Core)** — the committed `mingw` preset targets **Qt 6.5.0 mingw_64**. Other 6.x kits should work but are untested.
- A **C++23**-capable compiler.

### Using CMake presets (current setup)

The repository ships a `CMakePresets.json` with a `mingw` preset that configures a Ninja build against Qt 6.5.0 mingw_64 using Qt's bundled `mingw1120_64` toolchain (Debug, with `compile_commands.json` exported). This is the actively tested configuration:

```powershell
cmake --preset mingw
cmake --build --preset mingw
```

Build artifacts land in `build/mingw/`.

The preset pins absolute paths for `CMAKE_CXX_COMPILER` and `CMAKE_PREFIX_PATH` to a local Qt install under `T:/Qt`. Adjust those in `CMakePresets.json` to match your own Qt location, or set a `QTDIR` environment variable and derive from the hidden `Qt` preset (which reads `$env{QTDIR}`).

Because the samples and tests link the shared `MarkdownParser` library, Qt's `bin` directory **and** the bundled compiler's `bin` must be on `PATH` to run the resulting executables:

```powershell
$env:PATH = "T:\Qt\Tools\mingw1120_64\bin;T:\Qt\6.5.0\mingw_64\bin;$env:PATH"
```

### Build options

CMake options (all default `ON`):

| Option              | Description                                                                                        | Default |
| ------------------- | -------------------------------------------------------------------------------------------------- | ------- |
| `BUILD_SHARED_LIBS` | ON: build a shared library; OFF: build a static library.                                           | `ON`    |
| `BUILD_SAMPLES`     | Build the `samples` executable.                                                                    | `ON`    |
| `BUILD_TESTS`       | Build the `tests` executable.                                                                      | `ON`    |
| `CMAKE_PREFIX_PATH` | Path to find dependencies. Must point to the installation directory of the Qt kit you want to use. |         |

## Tests

Tests use the `QtTest` module and are data-driven (cases live in `tests/cases/section/*.json`). Run them with CTest:

```powershell
ctest --test-dir build/mingw --output-on-failure
```

The test executable is a GUI-subsystem program, so its `stdout` is not attached to the console — use CTest (or QtTest's `-o file` option) to read results rather than reading stdout directly. To run it directly with QtTest filtering:

```powershell
build/mingw/MarkdownParser_tests.exe                              # all cases
build/mingw/MarkdownParser_tests.exe testToHtml:"simple quote"    # one data row by tag
```

## Samples

The `run_samples` custom target converts every `.md` under `samples/samples/` (plus the top-level `README.md`) to HTML in `samples_output/`. It injects Qt's `bin` into `PATH` automatically:

```powershell
cmake --build --preset mingw --target run_samples
```

Be sure to save sample `.md` files as **UTF-8**.
