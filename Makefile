SOURCE_DIR=src
BUILD_DIR=_build

BINARY_DIR=binaries/arm64
BINARY=phi

DATE:=$(shell date "+%Y-%m-%d-%H:%M")
# to be used later when binaries are added into the binary dir



INC_DIRS=-Iinclude -I/opt/homebrew/include
# where UNcompiled headers are located

LIB_DIRS=-Llib -L/opt/homebrew/lib
# where compiled libraries are located

EXT_LIBS=
# external libraries; -lzlibcomplete=zlibcomplete, -lz=zlib, -lcryptopp=crypto++

STD=-std=c++17

OPT=-O2

DEP_FLAGS=-MMD -MP
# flags for generating dependancy .d files (for Make)

FINAL_FLAGS=-Wall -Wextra $(LIB_DIRS) $(EXT_LIBS) $(STD) $(OPT) -g
# -Wall=all warnings, -Wextra=extra warnings, -D for hardcoding env vars
OBJECT_FLAGS=-Wall -Wextra $(INC_DIRS) $(STD) $(OPT) $(DEP_FLAGS) -g



CPP_FILES_WITH_DIR=$(wildcard $(SOURCE_DIR)/*.cpp $(SOURCE_DIR)/**/*.cpp)

CPP_FILES=$(notdir $(CPP_FILES_WITH_DIR))
OBJECTS=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_FILES))
DEP_FILES=$(patsubst %.cpp, $(BUILD_DIR)/%.d, $(CPP_FILES))



#VPATH=$(sort $(dir $(CPP_FILES_WITH_DIR)))


all: $(BINARY)
	make format

$(BINARY): $(OBJECTS)
	clang++ $(FINAL_FLAGS) -o $@ $^
# -g=debugging information for lldb; replaces the "$@" with the binary and the "$^" with the object files

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp | $(BUILD_DIR) # any object file depends on its corresponding source file (or the build directory not existing)
	clang++ $(OBJECT_FLAGS) -c -o $@ $<
# -c=dont link into ELF; replaces the "$@" with the .o file and the "$<" with the .cpp file
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/**/%.cpp | $(BUILD_DIR)
	clang++ $(OBJECT_FLAGS) -c -o $@ $<


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


format:
	@find src -type f \( -name "*.c" -o -name "*.cpp" \) -exec clang-format -i {} +
	@find include -type f \( -name "*.h" -o -name "*.hpp" \) -exec clang-format -i {} +	 


clean-full:
	rm -f $(OBJECTS) $(DEP_FILES)

clean-obj:
	rm -f $(OBJECTS)

clean-dep:
	rm -f $(DEP_FILES)

archive:
	@if [ -f $(BINARY) ]; then \
       match_found=0; \
       for f in $(BINARY_DIR)/*; do \
           if [ -x "$$f" ] && cmp -s $(BINARY) "$$f"; then \
               echo "BINARY IS THE SAME AS ARCHIVED: $$f"; \
               match_found=1; \
               break; \
           fi; \
       done; \
       if [ $$match_found -eq 0 ]; then \
           echo "MOVING BINARY TO ARCHIVED BINARY: $(BINARY_DIR)/$(BINARY).$(DATE)"; \
           mv $(BINARY) $(BINARY_DIR)/$(BINARY).$(DATE); \
       else \
           rm $(BINARY); \
       fi; \
   fi
# archive binary if its not the same as any of the current archived binaries


.PHONY: all format clean-full clean-obj clean-dep archive

-include $(DEP_FILES)
