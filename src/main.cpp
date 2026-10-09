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

#define START 1
#define BLOCK 2
#define END   3
#define EMPTY 0

int window_width = 1200;
int window_height = 800;

float square_size = 100.0f;

SDL_Color color_red = {255, 0, 0};
SDL_Color color_green = {0, 255, 0};
SDL_Color color_blue = {0, 0, 255};
SDL_Color color_white = {255, 255, 255};

struct Tile {
    int type;

    float x = 0;
    float y = 0;

    int f = 0;
    int g = 0;
    int h = 0;

    SDL_Color color;
};

std::vector<Tile> grid = {
    Tile{.type = START, .color = color_blue},  Tile{.type = EMPTY, .color = color_white}, Tile{.type = EMPTY, .color = color_white},
    Tile{.type = EMPTY, .color = color_white}, Tile{.type = BLOCK, .color = color_red},   Tile{.type = EMPTY, .color = color_white},
    Tile{.type = BLOCK, .color = color_red},   Tile{.type = EMPTY, .color = color_white}, Tile{.type = END, .color = color_green},
};


void set_total_cost (Tile* tile, float g, float h) {
    tile->f = g + h;
}

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

    for (int i = 0; i < grid.size(); i++) {
        float x = w_start_pos + ((i % 3) * square_size); 
        float y = h_start_pos + ((i / 3) * square_size);

        grid[i].x = x;
        grid[i].y = y;
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

        for (int i = 0; i < grid.size(); i++) {
            draw_square(renderer, square_size, grid[i]); // Maybe passing the grid[i] ptr is better, may avoid copying the value, but Im not sure if this happens
        }


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_line(renderer, grid[0], grid[1]);
  
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Force 60 FPS -> 1000 (1s) / 60 = 16.6666...
    }
}
