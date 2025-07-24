#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <iostream>
#include <sstream>
#include "WaveRenderer.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const float FPS = 60.0f;

int main(int argc, char** argv) {
    // Initialize Allegro
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro" << std::endl;
        return -1;
    }
    
    // Initialize addons
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_ttf_addon();
    
    // Set OpenGL attributes
    al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_OPENGL_3_0);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
    
    // Create display
    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        std::cerr << "Failed to create display" << std::endl;
        return -1;
    }
    
    // Load font for UI
    ALLEGRO_FONT* font = al_create_builtin_font();
    if (!font) {
        std::cerr << "Failed to create font" << std::endl;
        al_destroy_display(display);
        return -1;
    }
    
    // Create event queue and timer
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    
    // Register event sources
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    
    // Initialize wave renderer
    WaveRenderer waveRenderer;
    if (!waveRenderer.initialize()) {
        std::cerr << "Failed to initialize wave renderer" << std::endl;
        al_destroy_font(font);
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        return -1;
    }
    
    // Game state
    bool running = true;
    bool redraw = true;
    float mouseX = 0, mouseY = 0;
    bool mousePressed = false;
    
    // Wave parameters
    float waveSpeed = 1.0f;
    float waveHeight = 0.2f;
    float waveFrequency = 5.0f;
    
    // Start timer
    al_start_timer(timer);
    
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                waveRenderer.update(1.0f / FPS);
                redraw = true;
                break;
                
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                running = false;
                break;
                
            case ALLEGRO_EVENT_KEY_DOWN:
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE:
                        running = false;
                        break;
                    case ALLEGRO_KEY_Q:
                        waveSpeed = std::min(waveSpeed + 0.1f, 5.0f);
                        waveRenderer.setWaveSpeed(waveSpeed);
                        break;
                    case ALLEGRO_KEY_A:
                        waveSpeed = std::max(waveSpeed - 0.1f, 0.1f);
                        waveRenderer.setWaveSpeed(waveSpeed);
                        break;
                    case ALLEGRO_KEY_W:
                        waveHeight = std::min(waveHeight + 0.05f, 1.0f);
                        waveRenderer.setWaveHeight(waveHeight);
                        break;
                    case ALLEGRO_KEY_S:
                        waveHeight = std::max(waveHeight - 0.05f, 0.05f);
                        waveRenderer.setWaveHeight(waveHeight);
                        break;
                    case ALLEGRO_KEY_E:
                        waveFrequency = std::min(waveFrequency + 0.5f, 20.0f);
                        waveRenderer.setWaveFrequency(waveFrequency);
                        break;
                    case ALLEGRO_KEY_D:
                        waveFrequency = std::max(waveFrequency - 0.5f, 1.0f);
                        waveRenderer.setWaveFrequency(waveFrequency);
                        break;
                }
                break;
                
            case ALLEGRO_EVENT_MOUSE_AXES:
                mouseX = event.mouse.x;
                mouseY = event.mouse.y;
                waveRenderer.setMousePosition(mouseX, mouseY);
                break;
                
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (event.mouse.button == 1) {
                    mousePressed = true;
                    waveRenderer.setMousePressed(true);
                }
                break;
                
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                if (event.mouse.button == 1) {
                    mousePressed = false;
                    waveRenderer.setMousePressed(false);
                }
                break;
        }
        
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            
            // Render wave
            waveRenderer.render(SCREEN_WIDTH, SCREEN_HEIGHT);
            
            // Draw UI overlay
            al_set_target_backbuffer(display);
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, 
                        "Wave Simulation with Shaders");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 30, 0, 
                        "Controls:");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 50, 0, 
                        "Q/A - Increase/Decrease Wave Speed");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 70, 0, 
                        "W/S - Increase/Decrease Wave Height");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 90, 0, 
                        "E/D - Increase/Decrease Wave Frequency");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 110, 0, 
                        "Mouse Click - Create Ripples");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 130, 0, 
                        "ESC - Exit");
            
            // Display current values
            std::stringstream ss;
            ss << "Speed: " << waveSpeed << " Height: " << waveHeight << " Frequency: " << waveFrequency;
            al_draw_text(font, al_map_rgb(255, 255, 0), 10, 160, 0, ss.str().c_str());
            
            al_flip_display();
        }
    }
    
    // Cleanup
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    
    return 0;
}