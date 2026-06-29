# TacoMake

TacoMake is a small, copy-and-use GNU Make setup for C and C++ projects that build one executable.

It is meant for projects that are too small for a full build system, but still need a clean workflow for source files, headers, object files, assets, external libraries, and platform-specific executable names.

## Features

- Builds a single executable from all source files under `src`
- Recursively discovers source files and mirrors their paths under `objs`
- Generates and includes dependency files with `-MMD -MP`
- Copies files from `assets` into `build` when running the program
- Supports internal and external include/library directories
- Adds `.exe` automatically on Windows
- Provides simple commands: `make`, `make run`, `make init`, `make clean`, and `make clean-all`

## Requirements

- GNU Make
- A C or C++ compiler, such as `g++`
- A shell environment where `make` can run

The project has mainly been used on Windows, and has also been tested on Debian GNU/Linux 12. Other Unix-like environments may work, but are not the primary target.

## Repository Layout

```text
TacoMake/
+-- main_files/
|   +-- Makefile
|   +-- tacomake/
|       +-- commands.mk
|       +-- functions.mk
|       +-- platform.mk
|       +-- rules.mk
+-- example/
|   +-- Makefile
|   +-- tacomake/
|   +-- src/
|   +-- include/
|   +-- assets/
+-- LICENSE
+-- README.md
```

`main_files` contains the files you copy into your own project.

`example` is a working sample project using the same build files.

## Quick Start With The Example

From the repository root:

```sh
cd example
make
```

This compiles the example project and creates the executable in `build`.

To build and run it:

```sh
make run
```

To remove generated object files, dependency files, and the executable:

```sh
make clean
```

To remove the full generated `objs` and `build` directories:

```sh
make clean-all
```

## Using TacoMake In Your Own Project

Create or open a C/C++ project directory, then copy these files from `main_files` into the root of your project:

```text
your-project/
+-- Makefile
+-- tacomake/
    +-- commands.mk
    +-- functions.mk
    +-- platform.mk
    +-- rules.mk
```

Your project should then look something like this:

```text
your-project/
+-- Makefile
+-- tacomake/
+-- src/
+-- include/
+-- lib/
+-- assets/
```

If the project folders do not exist yet, configure the directory names in `Makefile`, then run:

```sh
make init
```

This creates the directories configured by:

- `src_dir`
- `include_dir`
- `lib_dir`
- `assets_dir`

The `build` and `objs` directories are created automatically when needed.

## Basic Tutorial

This section walks through a minimal project from an empty directory.

### 1. Copy TacoMake Files

Copy `main_files/Makefile` and `main_files/tacomake` into your project root:

```text
hello-taco/
+-- Makefile
+-- tacomake/
```

### 2. Initialize The Folders

Run:

```sh
make init
```

You should now have:

```text
hello-taco/
+-- Makefile
+-- tacomake/
+-- src/
+-- include/
+-- lib/
+-- assets/
```

### 3. Add Source Code

Create `src/main.cpp`:

```cpp
#include <iostream>

int main()
{
    std::cout << "Hello from TacoMake!\n";
    return 0;
}
```

### 4. Build

Run:

```sh
make
```

On Windows, this creates:

```text
build/main.exe
```

On Linux, this creates:

```text
build/main
```

### 5. Run

Run:

```sh
make run
```

TacoMake enters the `build` directory and runs the executable from there.

### 6. Change The Executable Name

Open `Makefile` and edit:

```make
target := main
```

For example:

```make
target := hello
```

Now `make` will create `build/hello.exe` on Windows or `build/hello` on Linux.

## Makefile Configuration

Most configuration is done at the top of `Makefile`.

### Target And Compiler

```make
target := main
compiler := g++
cpp_ver := c++20
```

- `target` is the executable name without `.exe`.
- `compiler` is the compiler command.
- `cpp_ver` becomes the `-std=` flag, such as `-std=c++20`.

### Project Directories

```make
target_dir  := build
src_dir     := src
include_dir := include
lib_dir     := lib
obj_dir     := objs
assets_dir  := assets
```

- `src_dir` contains source files.
- `include_dir` contains project headers.
- `lib_dir` contains project libraries.
- `assets_dir` contains runtime files copied during `make run`.
- `target_dir` contains the final executable and copied assets.
- `obj_dir` contains generated object and dependency files.

### External Include And Library Directories

```make
ext_include_dir :=
ext_lib_dir     :=
```

Use these for directories outside your project.

Example:

```make
ext_include_dir := C:/libs/some-library/include
ext_lib_dir     := C:/libs/some-library/lib
```

These directories are not created by `make init`.

### Compiler Flags

```make
compile_flags := -g -Wall -Wextra -MMD -MP
```

These flags are passed when compiling each source file.

TacoMake also appends:

- `-std=$(cpp_ver)`
- `-I` flags for `include_dir` and `ext_include_dir`
- `-D` flags for every entry in `macros`

### Macros

```make
macros :=
```

Do not include `-D` yourself.

Example:

```make
macros := DEBUG USE_FAST_MATH
```

This becomes:

```text
-DDEBUG -DUSE_FAST_MATH
```

### Linker Flags And Libraries

```make
linker_flags :=
libraries :=
```

Use `linker_flags` for raw linker options.

Use `libraries` for library names. Do not include the `-l` prefix.

Example:

```make
libraries := pthread opengl32 gdi32
```

This becomes:

```text
-lpthread -lopengl32 -lgdi32
```

TacoMake also scans `lib_dir` and `ext_lib_dir` recursively and adds their subdirectories as `-L` paths.

### Source File Extensions

```make
src_suffix := .cpp .cxx .cp .c++ .CPP .cc .c .C
```

Files under `src_dir` with these suffixes are compiled. Most users should leave this unchanged.

## Commands

### `make`

Builds the executable.

This compiles source files from `src_dir`, writes object files to `obj_dir`, then links the executable into `target_dir`.

### `make run`

Builds the executable, copies assets from `assets_dir` into `target_dir`, then runs the executable from inside `target_dir`.

Use this when your program expects runtime files beside the executable.

### `make init`

Creates the basic project directories configured in `Makefile`:

- `src_dir`
- `include_dir`
- `lib_dir`
- `assets_dir`

### `make clean`

Deletes:

- the built executable
- generated object files
- generated dependency files

### `make clean-all`

Deletes the full generated output directories:

- `target_dir`
- `obj_dir`

## Asset Handling

Files inside `assets_dir` are copied to `target_dir` when you run:

```sh
make run
```

For example:

```text
assets/images/earthmap.jpg
```

is copied to:

```text
build/images/earthmap.jpg
```

Assets are not copied by plain `make`.

## Notes And Limitations

- TacoMake is designed for one executable target.
- The source file containing `main` must be somewhere under `src_dir`.
- Directory names with spaces are strongly discouraged.
- Empty directories are ignored by the recursive Make functions.
- Library link order can matter. If a library does not link correctly, adjust `libraries` or use `linker_flags`.
- The Windows support assumes GNU Make is running in a Windows environment where the `OS` variable is defined.

## License

TacoMake is released under the MIT License. See `LICENSE` for details.
