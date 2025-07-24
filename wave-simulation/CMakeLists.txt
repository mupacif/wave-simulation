cmake_minimum_required(VERSION 3.7)
project(WaveSimulation)

set(CMAKE_CXX_STANDARD 11)

# Set Allegro root (using the same as the parent project)
SET(ALLEGRO_ROOT ../allegro/)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Include directories
INCLUDE_DIRECTORIES(${ALLEGRO_ROOT}/include ${OPENGL_INCLUDE_DIRS} include)
LINK_DIRECTORIES(${ALLEGRO_ROOT}/lib)

# Source files
set(SOURCES
    src/main.cpp
    src/WaveRenderer.cpp
    src/ShaderManager.cpp
)

# Create executable
add_executable(wave_simulation ${SOURCES})

# Link libraries
target_link_libraries(wave_simulation 
    allegro
    allegro_main
    allegro_primitives
    allegro_font
    allegro_ttf
    ${OPENGL_LIBRARIES})

# Copy shaders to build directory
file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})