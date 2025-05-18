#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_SIZE 800
#define GRID_X 128
#define GRID_Y 128

static float** density = NULL;

void DrawFilledCircle(SDL_Renderer* renderer, int x0, int y0, int radius) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderLine(renderer, x0 - x, y0 + y, x0 + x, y0 + y);
        SDL_RenderLine(renderer, x0 - x, y0 - y, x0 + x, y0 - y);
        SDL_RenderLine(renderer, x0 - y, y0 + x, x0 + y, y0 + x);
        SDL_RenderLine(renderer, x0 - y, y0 - x, x0 + y, y0 - x);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void InitParticles() {
    density = malloc(GRID_X * sizeof(float*));
    for (int i = 0; i < GRID_X; ++i) {
        density[i] = malloc(GRID_Y * sizeof(float));
        for (int j = 0; j < GRID_Y; ++j) {
            // Example: wave pattern
            density[i][j] = 0.5f + 0.5f * sinf(i * 0.2f) * sinf(j * 0.2f);
        }
    }
}

void FreeParticles() {
    if (!density) return;
    for (int i = 0; i < GRID_X; ++i) {
        free(density[i]);
    }
    free(density);
    density = NULL;
}

void RenderDensity(SDL_Renderer* renderer, float** field, int width, int height) {
    float cellW = (float)WINDOW_SIZE / width;
    float cellH = (float)WINDOW_SIZE / height;

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            float d = field[i][j];
            if (d < 0.0f) d = 0.0f;
            if (d > 1.0f) d = 1.0f;

            Uint8 color = (Uint8)(d * 255);
            SDL_SetRenderDrawColor(renderer, color, color, color, 255);

            SDL_FRect rect = {
                .x = i * cellW,
                .y = j * cellH,
                .w = cellW,
                .h = cellH
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}




SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetAppMetadata("Fluid Simulation", "1.0", "com.example.fluid-renderer");

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return SDL_APP_FAILURE;

    if (SDL_CreateWindowAndRenderer("Fluid Grid", WINDOW_SIZE, WINDOW_SIZE, 0, &window, &renderer) < 0)
        return SDL_APP_FAILURE;

    InitParticles();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_SetRenderDrawColorFloat(renderer, 0.0f, 0.0f, 0.5f, 1.0f);
    SDL_RenderClear(renderer);

    RenderDensity(renderer, density, GRID_X, GRID_Y);

    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    FreeParticles();
    /* SDL handles cleanup for window/renderer */
}
