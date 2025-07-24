#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cmath>
#include <vector>
#include <sstream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const float FPS = 60.0f;
const int GRID_SIZE = 50;

struct WavePoint {
    float x, y, z;
    float height;
    ALLEGRO_COLOR color;
};

class WaveSimulation {
private:
    std::vector<std::vector<WavePoint>> grid;
    float time;
    float waveSpeed;
    float waveHeight;
    float waveFrequency;
    float mouseWaveX, mouseWaveY;
    float mouseWaveTime;
    bool mouseWaveActive;
    
public:
    WaveSimulation() : time(0), waveSpeed(1.0f), waveHeight(30.0f), 
                       waveFrequency(0.1f), mouseWaveTime(0), mouseWaveActive(false) {
        initGrid();
    }
    
    void initGrid() {
        grid.resize(GRID_SIZE);
        float spacing = 15.0f;
        float startX = SCREEN_WIDTH / 2 - (GRID_SIZE * spacing) / 2;
        float startY = SCREEN_HEIGHT / 2 - (GRID_SIZE * spacing) / 2;
        
        for (int i = 0; i < GRID_SIZE; i++) {
            grid[i].resize(GRID_SIZE);
            for (int j = 0; j < GRID_SIZE; j++) {
                grid[i][j].x = startX + j * spacing;
                grid[i][j].y = startY + i * spacing;
                grid[i][j].z = 0;
                grid[i][j].height = 0;
            }
        }
    }
    
    void update(float dt) {
        time += dt * waveSpeed;
        if (mouseWaveActive) {
            mouseWaveTime += dt * 5.0f;
            if (mouseWaveTime > 10.0f) {
                mouseWaveActive = false;
                mouseWaveTime = 0;
            }
        }
        
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                WavePoint& p = grid[i][j];
                
                // Base wave pattern
                float wave1 = sin(p.x * waveFrequency + time) * cos(p.y * waveFrequency + time * 0.8f);
                float wave2 = sin(p.x * waveFrequency * 1.7f + time * 1.3f) * sin(p.y * waveFrequency * 1.3f + time);
                
                // Mouse interaction wave
                float mouseWave = 0;
                if (mouseWaveActive) {
                    float dist = sqrt(pow(p.x - mouseWaveX, 2) + pow(p.y - mouseWaveY, 2));
                    mouseWave = sin(dist * 0.05f - mouseWaveTime) * exp(-dist * 0.005f) * 50.0f;
                }
                
                p.height = (wave1 * 0.5f + wave2 * 0.3f) * waveHeight + mouseWave;
                
                // Color based on height
                float normalizedHeight = (p.height + waveHeight) / (2 * waveHeight);
                float r = 0.0f;
                float g = 0.2f + normalizedHeight * 0.3f;
                float b = 0.4f + normalizedHeight * 0.4f;
                
                // Add foam on peaks
                if (p.height > waveHeight * 0.7f) {
                    float foam = (p.height - waveHeight * 0.7f) / (waveHeight * 0.3f);
                    r += foam * 0.9f;
                    g += foam * 0.9f;
                    b += foam * 0.5f;
                }
                
                p.color = al_map_rgb_f(r, g, b);
            }
        }
    }
    
    void render() {
        // Draw wave grid
        for (int i = 0; i < GRID_SIZE - 1; i++) {
            for (int j = 0; j < GRID_SIZE - 1; j++) {
                WavePoint& p1 = grid[i][j];
                WavePoint& p2 = grid[i][j + 1];
                WavePoint& p3 = grid[i + 1][j];
                WavePoint& p4 = grid[i + 1][j + 1];
                
                // Calculate screen positions with perspective
                float perspective = 0.002f;
                auto project = [perspective](WavePoint& p) -> std::pair<float, float> {
                    float scale = 1.0f / (1.0f + p.height * perspective);
                    return {p.x * scale, (p.y - p.height) * scale};
                };
                
                auto [x1, y1] = project(p1);
                auto [x2, y2] = project(p2);
                auto [x3, y3] = project(p3);
                auto [x4, y4] = project(p4);
                
                // Draw triangulated quad
                float vertices[] = {
                    x1, y1, x2, y2, x3, y3,
                    x2, y2, x3, y3, x4, y4
                };
                
                // Average color for the quad
                float r = (al_unmap_rgb_f(p1.color).r + al_unmap_rgb_f(p2.color).r + 
                          al_unmap_rgb_f(p3.color).r + al_unmap_rgb_f(p4.color).r) / 4;
                float g = (al_unmap_rgb_f(p1.color).g + al_unmap_rgb_f(p2.color).g + 
                          al_unmap_rgb_f(p3.color).g + al_unmap_rgb_f(p4.color).g) / 4;
                float b = (al_unmap_rgb_f(p1.color).b + al_unmap_rgb_f(p2.color).b + 
                          al_unmap_rgb_f(p3.color).b + al_unmap_rgb_f(p4.color).b) / 4;
                
                ALLEGRO_COLOR color = al_map_rgb_f(r, g, b);
                
                al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, color);
                al_draw_filled_triangle(x2, y2, x3, y3, x4, y4, color);
                
                // Draw wireframe
                al_draw_line(x1, y1, x2, y2, al_map_rgba_f(1, 1, 1, 0.2f), 1);
                al_draw_line(x1, y1, x3, y3, al_map_rgba_f(1, 1, 1, 0.2f), 1);
            }
        }
    }
    
    void createMouseWave(float x, float y) {
        mouseWaveX = x;
        mouseWaveY = y;
        mouseWaveTime = 0;
        mouseWaveActive = true;
    }
    
    void adjustSpeed(float delta) { waveSpeed = std::max(0.1f, std::min(5.0f, waveSpeed + delta)); }
    void adjustHeight(float delta) { waveHeight = std::max(5.0f, std::min(100.0f, waveHeight + delta)); }
    void adjustFrequency(float delta) { waveFrequency = std::max(0.02f, std::min(0.5f, waveFrequency + delta)); }
    
    float getSpeed() const { return waveSpeed; }
    float getHeight() const { return waveHeight; }
    float getFrequency() const { return waveFrequency; }
};

int main() {
    if (!al_init()) {
        return -1;
    }
    
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    
    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        return -1;
    }
    
    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    
    WaveSimulation wave;
    bool running = true;
    bool redraw = true;
    
    al_start_timer(timer);
    
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                wave.update(1.0f / FPS);
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
                        wave.adjustSpeed(0.1f);
                        break;
                    case ALLEGRO_KEY_A:
                        wave.adjustSpeed(-0.1f);
                        break;
                    case ALLEGRO_KEY_W:
                        wave.adjustHeight(5.0f);
                        break;
                    case ALLEGRO_KEY_S:
                        wave.adjustHeight(-5.0f);
                        break;
                    case ALLEGRO_KEY_E:
                        wave.adjustFrequency(0.01f);
                        break;
                    case ALLEGRO_KEY_D:
                        wave.adjustFrequency(-0.01f);
                        break;
                }
                break;
                
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (event.mouse.button == 1) {
                    wave.createMouseWave(event.mouse.x, event.mouse.y);
                }
                break;
        }
        
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            
            al_clear_to_color(al_map_rgb(20, 30, 50));
            
            wave.render();
            
            // Draw UI
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Wave Simulation (CPU-based)");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 30, 0, "Controls:");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 50, 0, "Q/A - Speed");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 70, 0, "W/S - Height");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 90, 0, "E/D - Frequency");
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 110, 0, "Click - Create Wave");
            
            std::stringstream ss;
            ss << "Speed: " << wave.getSpeed() << " Height: " << wave.getHeight() << " Freq: " << wave.getFrequency();
            al_draw_text(font, al_map_rgb(255, 255, 0), 10, 140, 0, ss.str().c_str());
            
            al_flip_display();
        }
    }
    
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    
    return 0;
}