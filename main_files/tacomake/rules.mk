ifndef TACO_RULES_MK
TACO_RULES_MK := 1

# ---- Unchangeable ----
override target := $(call fixexecutable,$(target))

# Append directory to target file
override build_target := $(target_dir)/$(target)

# Source files
override cpp_src := $(call rwildcard,$(src_dir),*.cpp)
override c_src   := $(call rwildcard,$(src_dir),*.c)
override srcs     := $(cpp_src) $(c_src)

# Object files
override objs := $(patsubst %.cpp,$(obj_dir)/%.o,$(cpp_src))
override objs += $(patsubst %.c,$(obj_dir)/%.o,$(c_src))

# Dependency files
override deps := $(objs:.o=.d)

# Determine final compile flags
override compile_flags += -std=$(cpp_ver) -I$(include_dir) $(addprefix -D,$(macros))

# Determine final link flags
override linker_flags += $(call libdirflags,$(lib_dir)) $(addprefix -l,$(libraries))

# Assets
override assets := $(call filewildcard,$(assets_dir))
override target_assets := $(assets:$(assets_dir)/%=$(target_dir)/%)
override target_assets_dirS := $(call extrdir,$(target_assets))

# Directories needed by object files and dependency files
override objs_mirror_dirs := $(call extrdir,$(objs))

# Integrate directories need to be created
override required_dirs := $(sort $(target_dir) $(target_assets_dirS) $(objs_mirror_dirs))

# Default target
all: $(build_target)

# Rule to link executable
$(build_target): $(objs) | $(target_dir)
	$(call msg,Starting linking...)
	@$(compiler) -o $@ $(objs) $(linker_flags)
	$(call msg,Building finished!)

# Rule to compile source files into object files
$(obj_dir)/%.o: %.cpp | $(objs_mirror_dirs)
	$(call msg,Compiling C++ source file to object file: $<)
	@$(compiler) $(compile_flags) -c $< -o $@
	$(call msg,Compiling finished!)

# Rule to compile C source files into object files
$(obj_dir)/%.o: %.c | $(objs_mirror_dirs)
	$(call msg,Compiling C source file to object file: $<)
	@$(compiler) $(compile_flags) -c $< -o $@
	$(call msg,Compiling finished!)

# Ensure the build directory exists
$(required_dirs):
	$(info Deteced missing directory "$@"$(comma) create new one...)
	@$(call mkdir,$(call fixpath, $@))


# Include dependency files if they exist
-include $(deps)

endif