UNAME_S := $(shell uname -s)

CXX = g++
CXXFLAGS = -Wall -Wextra -g -fsanitize=address -O0 -I$(PWD)/src
LDFLAGS = -fsanitize=address

# OS-specific flags
ifeq ($(UNAME_S), Darwin) # macOS
	LDFLAGS += -framework OpenGL -framework GLUT
else ifeq ($(UNAME_S), Linux) # Linux
	LDFLAGS += -lGL -lGLU -lglut
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW) # Windows MinGW
	LDFLAGS += -lopengl32 -lglu32 -lfreeglut
endif

# Directory structure
SRC_DIR = src/classes src/generator src/aux
SRC_FILES = $(wildcard $(addsuffix /*.cpp, $(SRC_DIR)))
OBJ_FILES = $(patsubst %.cpp, %.o, $(SRC_FILES))

# Generator files
GENERATOR_SRC_FILES = src/generator/generator.cpp src/generator/model.cpp src/aux/aux.cpp src/generator/plane.cpp
GENERATOR_OBJ_FILES = $(patsubst %.cpp, %.o, $(GENERATOR_SRC_FILES))
GENERATOR_EXECUTABLE = generator

# Engine files (define these if you need them)
ENGINE_SRC_FILES = src/engine/engine.cpp src/generator/model.cpp src/aux/aux.cpp
ENGINE_OBJ_FILES = $(patsubst %.cpp, %.o, $(ENGINE_SRC_FILES))
ENGINE_EXECUTABLE = engine

# Default target
all: $(GENERATOR_EXECUTABLE) $(ENGINE_EXECUTABLE) cleanup

# Generator target
$(GENERATOR_EXECUTABLE): $(GENERATOR_OBJ_FILES)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(ENGINE_EXECUTABLE): $(ENGINE_OBJ_FILES)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Generic rule for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ_FILES) $(GENERATOR_EXECUTABLE) *.3d

# Auto cleanup rule to remove .o files after successful build
.PHONY: cleanup
cleanup:
	@echo "Cleaning up object files..."
	@find . -name "*.o" -type f -delete