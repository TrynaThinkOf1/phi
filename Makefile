#=====# {{
# VARIABLES #

CXX:=clang++

DATE:=$(shell date "+%Y-%m-%d-%H:%M")
# to be used later when binaries are added into the binary dir

APP:=phi
APPD:=phid

SRC_DIR:=src
SHARED_DIR:=$(SRC_DIR)/shared
APP_DIR:=$(SRC_DIR)/$(APP)
APPD_DIR:=$(SRC_DIR)/$(APPD)

BUILD_DIR:=_build
APP_BUILD_DIR:=$(BUILD_DIR)/$(APP)
APPD_BUILD_DIR:=$(BUILD_DIR)/$(APPD)

#=====# }}


#=====# {{
# COMPILATION #
#    INPUT    #

INC_DIRS=-Iinclude -I/opt/homebrew/include
# where UNcompiled headers are located

LIB_DIRS=-Llib -L/opt/homebrew/lib
# where compiled libraries are located

EXT_LIBS=
# external libraries; -lzlibcomplete=zlibcomplete, -lz=zlib, -lcryptopp=crypto++

STD=-std=c++17

OPT=-O2

FINAL_FLAGS=-Wall -Wextra $(LIB_DIRS) $(EXT_LIBS) $(STD) $(OPT) -g
# -Wall=all warnings, -Wextra=extra warnings, -g=debugging info; -c=don't link into ELF
OBJECT_FLAGS=-Wall -Wextra $(INC_DIRS) $(STD) $(OPT) -g -c

#=====# }}


#=====# {{
#  FILENAME  #
#    LIST    #
# GENERATION #

SHARED_SRCS = $(wildcard $(SHARED_DIR)/*.cpp)

APP_SRCS = $(wildcard $(APP_DIR)/*.cpp)
APP_OBJS = $(patsubst $(APP_DIR)/%.cpp,$(APP_BUILD_DIR)/%.o,$(APP_SRCS)) \
           $(patsubst $(SHARED_DIR)/%.cpp,$(APP_BUILD_DIR)/%.o,$(SHARED_SRCS))

APPD_SRCS = $(wildcard $(APPD_DIR)/*.cpp)
APPD_OBJS = $(patsubst $(APPD_DIR)/%.cpp,$(APPD_BUILD_DIR)/%.o,$(APPD_SRCS)) \
            $(patsubst $(SHARED_DIR)/%.cpp,$(APPD_BUILD_DIR)/%.o,$(SHARED_SRCS))

#=====# }}


#=====# {{
# BUILD #
# RULES #

all: $(APP) $(APPD)
	@make format

$(APP): $(APP_OBJS) | $(APP_BUILD_DIR)
	$(CXX) $(FINAL_FLAGS) -o $@ $^
# $@ - rule object, $< - recursive dependency object
$(APPD): $(APPD_OBJS) | $(APPD_BUILD_DIR)
	$(CXX) $(FINAL_FLAGS) -o $@ $^

#===# {
#  ACTUAL  #
# BUILDING #
$(APP_BUILD_DIR)/%.o: $(APP_DIR)/%.cpp $(APP_BUILD_DIR)
	$(CXX) $(OBJECT_FLAGS) -o $@ $<
$(APP_BUILD_DIR)/%.o: $(SHARED_DIR)/%.cpp $(APP_BUILD_DIR)
	$(CXX) $(OBJECT_FLAGS) -o $@ $<
# $@ - rule object, $< - dependency object
$(APPD_BUILD_DIR)/%.o: $(APPD_DIR)/%.cpp $(APPD_BUILD_DIR)
	$(CXX) $(OBJECT_FLAGS) -o $@ $<
$(APPD_BUILD_DIR)/%.o: $(SHARED_DIR)/%.cpp $(APPD_BUILD_DIR)
	$(CXX) $(OBJECT_FLAGS) -o $@ $<
#===# }

#===# {
# SUPPORT #
$(APP_BUILD_DIR):
	mkdir -p $(APP_BUILD_DIR)

$(APPD_BUILD_DIR):
	mkdir -p $(APPD_BUILD_DIR)
#===# }

#=====# }}


#=====# {{
# OTHER #
# RULES #

format:
	@find src -type f \( -name "*.c" -o -name "*.cpp" \) -exec clang-format -i {} +
	@find include -type f \( -name "*.h" -o -name "*.hpp" \) -exec clang-format -i {} +	 

clean:
	rm -rf $(BUILD_DIR)/*

#=====# }}

.PHONY: all format clean
