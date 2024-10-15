# TacoMake

Generic C++ makefile for medium or simple single executable compilation.

# README for TacoMake Project

Welcome to the **TacoMake** project! This guide will help you set up and use the build system efficiently. Follow along to learn how to configure your environment and take advantage of various make commands to manage your project.

## Prerequisites

- **GNU Make** version **4.4.1** or higher is required.
- This project is well-suited for development in **VS Code** as a C++ project due to its convenient environment for managing multiple files and its compatibility with the `make` utility.

## Setup Instructions

1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```
2. Ensure the `Makefile` is at the root level and all other files are in the `tacomake` folder.
3. Or download the project as a ZIP file from the GitHub website by clicking on the "Code" button and selecting "Download ZIP". Extract the contents after downloading.

## Project Structure

The repository contains two key folders:

- **main\_files**: This folder includes the main makefile components and instructions.
- **example**: Contains code examples for learning purposes, particularly from the "Raytracing The Rest of Your Life" series.

Upon cloning the repository, you will find the following structure:

```
main_files/
    tacomake/
        commands.mk
        functions.mk
        platform.mk
        rules.mk
    Makefile
example/
    (Code for Ray Tracing tutorials)
    (Constructed using this makefile system)
```

## File Placement

- All files except `Makefile` should be placed inside a folder named `tacomake`. This is to ensure that the configuration makefile code is kept separate and independent.
- The `Makefile` file should be placed in the root directory, outside of the `tacomake` folder.

## Configuring `Makefile`

The `Makefile` file allows you to configure essential paths and settings:

- **Directories**:

  - `src_dir`: Directory for source files. This directory is where your main source code files are located.
    - *Example*: `src_dir := src`
  - `include_dir`: Directory for header files. Include files are placed here for organizing reusable code.
    - *Example*: `include_dir := include`
  - `assets_dir`: Directory containing resources for the program, such as images, configuration files, and dynamic link libraries (`.dll` or `.so`). During `make run`, these assets will be copied to the `build` directory.
    - *Example*: `assets_dir := assets`
  - `target_dir`: Directory for build output. This is where the compiled executable will be placed.
    - *Example*: `target_dir := build`
  - `obj_dir`: Directory for compiled object files. Keeping object files separate helps manage the compilation process more cleanly.
    - *Example*: `obj_dir := objs`
  - `lib_dir`: Directory for libraries used by the program. This is where additional static or dynamic libraries can be placed for linking.
    - *Example*: `lib_dir := lib`

  Note: All directories in `lib_dir` will automatically be added with `-L` during the linking stage. Therefore, you only need to specify the library names in the `libraries` variable.

- **Compiler Settings**:

  - `compiler`: Specifies the compiler to use (e.g., `g++`).
    - *Example*: `compiler := g++`
  - `cpp_ver`: Sets the C++ version standard (e.g., `c++20`).
    - *Example*: `cpp_ver := c++20`
  - `compile_flags`: Flags for the compiler to control warnings and debugging.
    - *Example*: `compile_flags := -g -Wall -Wextra -MMD -MP`
  - `macros`: Defines any macros that should be passed to the compiler. Note that the values in `macros` will automatically be prefixed with `-D`, so you only need to provide the macro name.
    - *Example*: `macros := DEBUG`
  - `linker_flags`: Flags for the linker, if additional options are needed during linking.
    - *Example*: `linker_flags := -static-libgcc`
  - `libraries`: Specifies any libraries to link against, which are automatically prefixed with `-l` (e.g., `pthread`).
    - *Example*: `libraries := pthread opengl32 gdi32`
      Note: Only add the library names in the `libraries` variable without the `lib` prefix or `.a`/`.lib` suffix, such as `opengl32` or `gdi32`.

Modify these paths and settings as needed to match your project structure.

## Make Commands

This project offers five key `make` commands to simplify your workflow:

### 1. `make init`

Initializes the working environment. This command creates the necessary directories defined in `Makefile`. Ensure you have set up `src_dir`, `include_dir`, `lib_dir`, `obj_dir`, `target_dir`, and `assets_dir` appropriately before running this command.

### 2. `make`

Compiles the source files to generate an executable without running it. Note that assets are **not** copied to the build directory during this step.

### 3. `make run`

Compiles and runs the project. Before running, it also copies any updated assets from the `assets` directory to the `build` directory, including dynamic link libraries (`.dll` for Windows or `.so` for Linux). Use this to quickly compile and test the project.

### 4. `make clean`

Removes all object files and the generated executable, providing a clean state for recompilation.

### 5. `make clean-all`

Deletes both `objs` and `build` folders, effectively removing everything generated during the build process. This is equivalent to removing the folders specified in `obj_dir` and `target_dir`.

## Using the Example Code

In the **example** folder, you'll find source code from the popular **"Ray Tracing The Rest of Your Life"** tutorial series. This code is ideal for learning how to utilize the `make` commands effectively:

- Use `make` to compile the example code.
- Execute `make run` to compile and run it, observing how assets are handled.
- Test the **`make clean`** and **`make clean-all`** commands to understand their effects.

## Additional Examples

Consider adding more practical examples to illustrate different use cases of this build system. For instance, include a simple C++ project that highlights how to link multiple libraries or how to organize a larger codebase. Providing these examples will make it easier for new users to understand the full potential of `TacoMake`.

## Target Audience

This project is primarily targeted at C++ developers working on small to medium-sized projects who are looking for a simple and efficient way to manage their build process. It is particularly useful for developers who are new to using `make` or those who need a lightweight solution for single executable projects.

## Important Notes

- This makefile system only allows compiling a single executable file. The source code containing the `main` function must be placed in the directory specified by `src_dir` for it to be detected.
- Since this project uses native `make` functions to traverse directories, empty directories will be ignored. Please ensure that the absence of empty directories is acceptable for your use case.
- This makefile system has been thoroughly tested only on **Windows 11**. It should work on some Linux systems (tested successfully on **Debian GNU/Linux 12**), but extensive testing on other environments has not been conducted.

## Contributing

Feel free to fork and modify this project to suit your needs. Pull requests are welcome if you have improvements or additional features you'd like to share. However, please note that I may not actively monitor updates, so you might not receive a prompt response. Also, since English is not my native language, my responses might be delayed.

When contributing, please ensure that your changes are well-documented and thoroughly tested. If you are adding new functionality, consider including example usage to demonstrate its utility.

## License

This project is open source under the MIT license. See the LICENSE file for more details.

## Contact

For any questions, please create an issue in the repository or contact the project maintainer.

If you have specific feedback or need further clarification on using `TacoMake`, feel free to reach out. I'm always interested in hearing how others are using this system and what improvements could be made.
