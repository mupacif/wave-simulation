CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf -lm

TARGET = wave_simulation_simple

all: $(TARGET)

$(TARGET): wave_simple.cpp
	$(CXX) $(CXXFLAGS) wave_simple.cpp -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run