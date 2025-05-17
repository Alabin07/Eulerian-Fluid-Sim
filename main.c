#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define GRID_WIDTH 64
#define GRID_HEIGHT 64
#define WINDOW_SIZE 800

float** create_density_grid(int width, int height) {
    float** grid = malloc(width * sizeof(float*));
    for (int i = 0; i < width; i++) {
        grid[i] = malloc(height * sizeof(float));
        for (int j = 0; j < height; j++) {
            // Example: simple wave pattern
            grid[i][j] = 0.5f + 0.5f * sinf(i * 0.2f) * sinf(j * 0.2f);
        }
    }
    return grid;
}

void free_density_grid(float** grid, int width) {
    for (int i = 0; i < width; i++) {
        free(grid[i]);
    }
    free(grid);
}

void render_density(SDL_Renderer* renderer, float** density, int rows, int cols) {
    float cellWidth = (float)WINDOW_SIZE / cols;
    float cellHeight = (float)WINDOW_SIZE / rows;

    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            float d = density[i][j];
            if (d > 1.0f) d = 1.0f;
            if (d < 0.0f) d = 0.0f;

            Uint8 color = (Uint8)(d * 255);
            SDL_SetRenderDrawColor(renderer, color, color, color, 255);

            SDL_FRect rect = {
                .x = i * cellWidth,
                .y = j * cellHeight,
                .w = cellWidth,
                .h = cellHeight
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Fluid Grid",
                                          WINDOW_SIZE, WINDOW_SIZE,
                                          SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    float** density = create_density_grid(GRID_WIDTH, GRID_HEIGHT);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render_density(renderer, density, GRID_HEIGHT, GRID_WIDTH);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);  // ~60 FPS
    }

    free_density_grid(density, GRID_WIDTH);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
