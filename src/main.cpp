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
#include <SDL3_ttf/SDL_ttf.h>
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

int grid_dimension = 3;

std::vector<Tile> grid = {
    Tile{.type = START, .color = color_blue},  Tile{.type = EMPTY, .color = color_white}, Tile{.type = EMPTY, .color = color_white},
    Tile{.type = EMPTY, .color = color_white}, Tile{.type = BLOCK, .color = color_red},   Tile{.type = EMPTY, .color = color_white},
    Tile{.type = BLOCK, .color = color_red},   Tile{.type = EMPTY, .color = color_white}, Tile{.type = END, .color = color_green},
};

void print_tile(Tile tile) {
    print("Tile " + std::to_string(tile.x) + "x" + std::to_string(tile.y) + "; Type: " + std::to_string(tile.type) + "; g: " + std::to_string(tile.g) + " h: " + std::to_string(tile.h) + " f: " + std::to_string(tile.f));
}

float get_center(float a) {
    return a + (square_size / 2);
}

void set_total_cost (Tile* tile, float g, float h) {
    tile->f = g + h;
}

void draw_square(SDL_Renderer* renderer, TTF_Font* font, TTF_TextEngine* engine, float size, Tile tile) {
    SDL_SetRenderDrawColor(renderer, tile.color.r, tile.color.g, tile.color.b, 255);
    SDL_FRect square = { tile.x, tile.y, size, size };
    SDL_RenderRect(renderer, &square);

    TTF_Text* F = TTF_CreateText(engine, font, ("F: " + std::to_string(tile.f)).c_str(), 0);
    TTF_Text* G = TTF_CreateText(engine, font, ("G: " + std::to_string(tile.g)).c_str(), 0);
    TTF_Text* H = TTF_CreateText(engine, font, ("H: " + std::to_string(tile.h)).c_str(), 0);

    int bottom_offset = (size - 16);
    int right_offset = (size - 42);

    TTF_DrawRendererText(F, tile.x, tile.y);
    TTF_DrawRendererText(G, tile.x, tile.y + bottom_offset);
    TTF_DrawRendererText(H, tile.x + right_offset, tile.y + bottom_offset);
    TTF_DestroyText(F);
    TTF_DestroyText(G);
    TTF_DestroyText(H);
}

void draw_line(SDL_Renderer* renderer, Tile origin, Tile destination) {
    float x1 = get_center(origin.x); 
    float y1 = get_center(origin.y);

    float x2 = get_center(destination.x);
    float y2 = get_center(destination.y);

    SDL_RenderLine(renderer, x1, y1, x2, y2);
}

Tile* get_tile_at(std::vector<Tile>& grid, int row, int col, int dimension) {
    if (row < 0 || row >= dimension || col < 0 || col >= dimension) {
        return nullptr;
    }
    
    return &grid[row * dimension + col];
}

float calc_heuristic_manhattan(Tile* tile_a, Tile* tile_b) {
    return std::abs(get_center(tile_a->x) - get_center(tile_b->x)) + std::abs(get_center(tile_a->y) - get_center(tile_b->y));
}

void set_distance_params(Tile* tile, Tile* goal, int g) {
    if (tile == nullptr) return;

    float h = calc_heuristic_manhattan(tile, goal); // temp for testing
    float f = g + h;

    tile->g = g;
    tile->h = h;
    tile->f = f;
}

int main(int arhc, char* argv[]) {
    print("Starting");

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("A* algorithm test", window_width, window_height, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("./roboto.ttf", 12.0f);
    TTF_TextEngine* engine = TTF_CreateRendererTextEngine(renderer);

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

    Tile& goal = grid[grid.size() - 1]; // Assuming that the last item is the goal

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

        int i = 0; 

        int row = i / grid_dimension;
        int col = i % grid_dimension;
       
        Tile* start = &grid[i];

        Tile* tile_nb_right = get_tile_at(grid, row, col + 1, grid_dimension);
        Tile* tile_nb_left = get_tile_at(grid, row, col - 1, grid_dimension); 
        Tile* tile_nb_top = get_tile_at(grid, row - 1, col, grid_dimension);
        Tile* tile_nb_bottom = get_tile_at(grid, row + 1, col, grid_dimension);

        float g = i + 1; // temp for testing
        set_distance_params(tile_nb_right, &goal, g);
        set_distance_params(tile_nb_left, &goal, g);
        set_distance_params(tile_nb_top, &goal, g);
        set_distance_params(tile_nb_bottom, &goal, g);
        // print_tile(*tile_nb_right);
        //print_tile(*tile_nb_left);
        //print_tile(*tile_nb_top);
        // print_tile(*tile_nb_bottom);


        // h -> distance between the current tile and the goal
        // g -> distance between the starting point and the current tile
        // TODO: Calculate f and g, then save those informations and continue with the algorithm
        // Ref video: https://www.youtube.com/watch?v=o5_mqZKhTvw

        if (exit) {
            break;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < grid.size(); i++) {
            draw_square(renderer, font, engine, square_size, grid[i]); // Maybe passing the grid[i] ptr is better, may avoid copying the value, but Im not sure if this happens
        }


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_line(renderer, grid[0], grid[1]);
  
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Force 60 FPS -> 1000 (1s) / 60 = 16.6666...
    }
}
