#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <stdio.h>
#include <algorithm>

#include "logger.h" 

int window_width = 1200;
int window_height = 800;

SDL_Color color_red = {255, 0, 0};
SDL_Color color_green = {0, 255, 0};
SDL_Color color_blue = {0, 0, 255};
SDL_Color color_white = {255, 255, 255};

float square_size = 100.0f;

struct Tile {
    SDL_Color color;
    float x;
    float y;
};

// 1: start; 2: block; 3: end;
std::vector<int> grid = {
    1, 0, 0,
    0, 2, 0,
    2, 0, 3,
};

void draw_square(SDL_Renderer* renderer, float size, Tile tile) {
    SDL_SetRenderDrawColor(renderer, tile.color.r, tile.color.g, tile.color.b, 255);
    SDL_FRect square = { tile.x, tile.y, size, size };
    SDL_RenderRect(renderer, &square);
}

void draw_line(SDL_Renderer* renderer, Tile origin, Tile destination) {
    float x1 = origin.x + (square_size / 2); 
    float y1 = origin.y + (square_size / 2);

    float x2 = destination.x + (square_size / 2);
    float y2 = destination.y + (square_size / 2);

    SDL_RenderLine(renderer, x1, y1, x2, y2);
}

int main(int arhc, char* argv[]) {
    print("Starting");

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("A* algorithm test", window_width, window_height, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    SDL_Event event;
    bool exit = false;

    Uint64 frameStart = -1;

    // Init
    float w_start_pos = 300;
    float h_start_pos = 200;


    std::vector<Tile> tiles = {};
    
    for (int i = 0; i < grid.size(); i++) {
        float x = w_start_pos + ((i % 3) * square_size); 
        float y = h_start_pos + ((i / 3) * square_size);

        if (grid[i] == 1) {
            tiles.push_back(Tile{color_blue, x, y});
            continue;
        }

        if (grid[i] == 2) {
            tiles.push_back(Tile{color_red, x, y});
            continue;
        }

        if (grid[i] == 3) {
            tiles.push_back(Tile{color_green, x, y});
            continue;
        }

        tiles.push_back(Tile{color_white, x, y});
    }

    // Main Loop
    while(true) {
        Uint64 ticks = SDL_GetTicks();
        float dt = (ticks - frameStart) / 1000.0;
        frameStart = ticks;

        // Update
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                exit = true;
            }
        }

        if (exit) {
            break;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < tiles.size(); i++) {
            draw_square(renderer, square_size, tiles[i]);
        }


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_line(renderer, tiles[0], tiles[1]);
  
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Force 60 FPS -> 1000 (1s) / 60 = 16.6666...
    }
}
