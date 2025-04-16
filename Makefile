UNAME_S := $(shell uname -s)

CXX = g++ 
CXXFLAGS =-std=c++11 -Wall -Wextra -g -fsanitize=address -O3 -I$(PWD)/src

# OS-specific flags
ifeq ($(UNAME_S), Darwin) # macOS
	CXXFLAGS += -DGL_SILENCE_DEPRECATION
	LDFLAGS = -fsanitize=address -framework OpenGL -framework GLUT
else ifeq ($(UNAME_S), Linux) # Linux
	LDFLAGS = -fsanitize=address -lGL -lGLU -lglut -lGLEW
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW) # Windows MinGW
	LDFLAGS = -fsanitize=address -lopengl32 -lglu32 -lfreeglut
endif

# Directory structure
SRC_DIR = src/classes src/generator src/aux
SRC_FILES = $(wildcard $(addsuffix /*.cpp, $(SRC_DIR)))
OBJ_FILES = $(patsubst %.cpp, %.o, $(SRC_FILES))

# Generator files
GENERATOR_SRC_FILES = src/generator/generator.cpp src/generator/model.cpp src/aux/aux.cpp src/aux/curves.cpp src/generator/plane.cpp src/generator/box.cpp src/generator/sphere.cpp src/generator/cone.cpp src/generator/cylinder.cpp src/generator/bezier.cpp
GENERATOR_OBJ_FILES = $(patsubst %.cpp, %.o, $(GENERATOR_SRC_FILES))
GENERATOR_EXECUTABLE = .generator

# Engine files (define these if you need them)
ENGINE_SRC_FILES = src/engine/engine.cpp src/generator/model.cpp src/aux/aux.cpp src/xml/xml_parser.cpp src/xml/tinyxml2.cpp src/aux/curves.cpp
ENGINE_OBJ_FILES = $(patsubst %.cpp, %.o, $(ENGINE_SRC_FILES))
ENGINE_EXECUTABLE = .engine

# Total number of files for progress calculation
TOTAL_FILES := $(words $(sort $(GENERATOR_SRC_FILES) $(ENGINE_SRC_FILES)))

# Default target
all: 
	@echo "Compiling $(TOTAL_FILES) source files...\n"
	@$(MAKE) --no-print-directory build_with_progress
	@echo "Compilation complete!"

build_with_progress: reset_progress $(GENERATOR_EXECUTABLE) $(ENGINE_EXECUTABLE) cleanup

# Reset progress counter
reset_progress:
	@echo 0 > .progress

# Generator target
$(GENERATOR_EXECUTABLE): $(GENERATOR_OBJ_FILES)
	@$(CXX) $^ -o $@ $(LDFLAGS)

$(ENGINE_EXECUTABLE): $(ENGINE_OBJ_FILES)
	@$(CXX) $^ -o $@ $(LDFLAGS)

# Generic rule for object files with progress tracking
%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@count=$$(cat .progress); \
	count=$$((count+1)); \
	echo $$count > .progress; \
	percentage=$$((count * 100 / $(TOTAL_FILES))); \
	current_file=$$(basename $<); \
	printf "\r%-100s" ""; \
	if [ $$percentage -eq 100 ]; then \
		printf "\r  %3d%% (%d/%d) " $$percentage $$count $(TOTAL_FILES); \
		printf "%0.s█" $$(seq 1 50); \
		printf " All done!\n"; \
	else \
		bar_filled=$$((percentage / 2)); \
		bar_empty=$$((50 - bar_filled)); \
		printf "\r  %3d%% (%d/%d) " $$percentage $$count $(TOTAL_FILES); \
		printf "%0.s█" $$(seq 1 $$bar_filled); \
		printf "%0.s " $$(seq 1 $$bar_empty); \
		printf " Compiling %s" $$current_file; \
	fi

# Clean rule
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJ_FILES) $(GENERATOR_EXECUTABLE) $(ENGINE_EXECUTABLE) .progress

clean3d:
	@echo "Cleaning up 3D files..."
	@rm -f *.3d

# Auto cleanup rule to remove .o files after successful build
.PHONY: cleanup
cleanup:
	@echo "\nCleaning up object files..."
	@find . -name "*.o" -type f -delete
	@rm -f .progress