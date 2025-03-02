CXX = g++
SRC_DIR = src/classes src/generator
SRC_FILES = $(wildcard $(addsuffix /*.cpp, $(SRC_DIR)))
OBJ_FILES = $(patsubst %.cpp, %.o, $(SRC_FILES))

GENERATOR_SRC_FILES = src/generator/generator.cpp src/generator/box.cpp src/generator/plane.cpp src/generator/sphere.cpp
GENERATOR_OBJ_FILES = $(patsubst %.cpp, %.o, $(GENERATOR_SRC_FILES))

GENERATOR_EXECUTABLE = generator

all: $(GENERATOR_EXECUTABLE)

$(GENERATOR_EXECUTABLE): $(GENERATOR_OBJ_FILES) src/classes/figure.o src/classes/list.o src/classes/point.o
	$(CXX) $^ -o $@ -lGL -lGLU -lglut

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) $(GENERATOR_EXECUTABLE)