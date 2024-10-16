####################################################
# Compile
####################################################
# Target file
target := PathTracer

# CPP compiler
compiler := g++

# CPP version
cpp_ver := c++20

# Directories
target_dir  := build
src_dir     := src
include_dir := include
lib_dir     := lib
obj_dir     := objs

# Directory contains resources for program
assets_dir := assets

# Compiler flags
compile_flags := -g -Wall -Wextra -MMD -MP

# Define Macros
macros := 

# Linker flags
linker_flags := 
# Link libraries, this will be append -l prefix automatically. For example, "gdi32" will become -lgdi32.
# So just type the libraries name you want to link, like "gdi32 opengl32 pthread".
libraries := 


# Source file suffix configurations (Not recommended to change)
src_suffix := .cpp .cxx .cc .cp .c++ .c


####################################################
####################################################
include tacomake/platform.mk
include tacomake/functions.mk
include tacomake/rules.mk
include tacomake/commands.mk