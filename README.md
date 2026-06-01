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

- cmake: version **3.19** minimum (required for CMake presets)
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