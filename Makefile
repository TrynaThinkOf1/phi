SOURCE_DIR=src
BUILD_DIR=_build

BINARY_DIR=binaries/arm64
BINARY1=phi
BINARY2=phid

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

FINAL_FLAGS=-Wall -Wextra $(LIB_DIRS) $(EXT_LIBS) $(STD) $(OPT) -g
# -Wall=all warnings, -Wextra=extra warnings, -g for debugging info
OBJECT_FLAGS=-Wall -Wextra $(INC_DIRS) $(STD) $(OPT) -g



CPP_FILES_WITH_DIR1=$(wildcard $(SOURCE_DIR)/phi/*.cpp $(SOURCE_DIR)/phi/**/*.cpp)

CPP_FILES1=$(notdir $(CPP_FILES_WITH_DIR1))
OBJECTS1=$(patsubst %.cpp, $(BUILD_DIR)/phi/%.o, $(CPP_FILES1))


CPP_FILES_WITH_DIR2=$(wildcard $(SOURCE_DIR)/phid/*.cpp $(SOURCE_DIR)/phid/**/*.cpp)

CPP_FILES2=$(notdir $(CPP_FILES_WITH_DIR2))
OBJECTS2=$(patsubst %.cpp, $(BUILD_DIR)/phid/%.o, $(CPP_FILES2))



all: $(BINARY1) | $(BINARY2)
	make format

$(BINARY1): $(OBJECTS1)
	clang++ $(FINAL_FLAGS) -o $@ $^
# replaces the "$@" with the binary and the "$^" with the object files

$(BUILD_DIR)/phi/%.o: $(SOURCE_DIR)/phi/%.cpp | $(BUILD_DIR)/phi
	clang++ $(OBJECT_FLAGS) -c -o $@ $<
# -c=dont link into ELF; replaces the "$@" with the .o file and the "$<" with the .cpp file
$(BUILD_DIR)/phi/%.o: $(SOURCE_DIR)/phi/**/%.cpp | $(BUILD_DIR)/phi
	clang++ $(OBJECT_FLAGS) -c -o $@ $<


$(BINARY2): $(OBJECTS2)
	clang++ $(FINAL_FLAGS) -o $@ $^
# replaces the "$@" with the binary and the "$^" with the object files

$(BUILD_DIR)/phid/%.o: $(SOURCE_DIR)/phid/%.cpp | $(BUILD_DIR)/phid
	clang++ $(OBJECT_FLAGS) -c -o $@ $<
# -c=dont link into ELF; replaces the "$@" with the .o file and the "$<" with the .cpp file
$(BUILD_DIR)/phid/%.o: $(SOURCE_DIR)/phid/**/%.cpp | $(BUILD_DIR)/phid
	clang++ $(OBJECT_FLAGS) -c -o $@ $<


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
$(BUILD_DIR)/phi:
	mkdir -p $(BUILD_DIR)/phi
$(BUILD_DIR)/phid:
	mkdir -p $(BUILD_DIR)/phid


format:
	@find src -type f \( -name "*.c" -o -name "*.cpp" \) -exec clang-format -i {} +
	@find include -type f \( -name "*.h" -o -name "*.hpp" \) -exec clang-format -i {} +	 


clean:
	rm -f $(BUILD_DIR)/phi/* $(BUILD_DIR)/phid/*


archive:
	@if [ -f $(BINARY1) ]; then \
       match_found=0; \
       for f in $(BINARY_DIR)/phi/*; do \
           if [ -x "$$f" ] && cmp -s $(BINARY1) "$$f"; then \
               echo "BINARY '$(BINARY1)' IS THE SAME AS ARCHIVED: $$f"; \
               match_found=1; \
               break; \
           fi; \
       done; \
       if [ $$match_found -eq 0 ]; then \
           echo "MOVING BINARY '$(BINARY1)' TO ARCHIVED BINARY: $(BINARY_DIR)/phi/$(BINARY1).$(DATE)"; \
           mv $(BINARY1) $(BINARY_DIR)/phi/$(BINARY1).$(DATE); \
       else \
           rm $(BINARY1); \
       fi; \
	else \
		echo "NO BINARY '$(BINARY1)' FOUND"; \
	fi
	@if [ -f $(BINARY2) ]; then \
		match_found=0; \
		for f in $(BINARY_DIR)/phid/*; do \
			if [ -x "$$f" ] && cmp -s $(BINARY2) "$$f"; then \
				echo "BINARY '$(BINARY2)' IS THE SAME AS ARCHIVED: $$f"; \
				match_found=1; \
				break; \
			fi; \
		done; \
		if [ $$match_found -eq 0 ]; then \
			echo "MOVING BINARY '$(BINARY2)' TO ARCHIVED BINARY: $(BINARY_DIR)/phid/$(BINARY2).$(DATE)"; \
			mv $(BINARY2) $(BINARY_DIR)/phid/$(BINARY2).$(DATE); \
		else \
			rm $(BINARY2); \
		fi; \
	else \
		echo "NO BINARY '$(BINARY2)' FOUND"; \
	fi
# archive binary if its not the same as any of the current archived binaries


.PHONY: all format clean archive
