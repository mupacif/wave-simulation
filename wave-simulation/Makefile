CXX = g++
CXXFLAGS = -std=c++11 -Wall -I../allegro/include -Iinclude
LDFLAGS = -L../allegro/lib
LIBS = -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf -lGL -lGLU -lGLEW

SRCDIR = src
OBJDIR = obj
BINDIR = build

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
TARGET = $(BINDIR)/wave_simulation

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)
	@cp -r shaders $(BINDIR)/

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

run: all
	cd $(BINDIR) && ./wave_simulation

.PHONY: all clean directories run