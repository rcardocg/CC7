#include "parkingGUI.h"
#include <SDL2/SDL.h>
#include <pthread.h>

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 250

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;

static pthread_mutex_t gui_mutex = PTHREAD_MUTEX_INITIALIZER;

static int current_available = 0;
static int current_total = 0;

bool gui_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return false;
    }

    window = SDL_CreateWindow(
        "Parking Lot Status",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!window) {
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    return true;
}

void gui_set_spaces(int available, int total)
{
    pthread_mutex_lock(&gui_mutex);
    current_available = available;
    current_total = total;
    pthread_mutex_unlock(&gui_mutex);
}

bool gui_process_frame(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        }
    }

    pthread_mutex_lock(&gui_mutex);
    int available = current_available;
    int total = current_total;
    pthread_mutex_unlock(&gui_mutex);

    int occupied = total - available;

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);

    // Dibujar 3 espacios
    for (int i = 0; i < total; i++) {
        SDL_Rect box = {60 + i * 130, 80, 90, 90};

        if (i < occupied) {
            // rojo = ocupado
            SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);
        } else {
            // verde = libre
            SDL_SetRenderDrawColor(renderer, 60, 180, 75, 255);
        }

        SDL_RenderFillRect(renderer, &box);

        // borde negro
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &box);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16); // ~60 FPS
    return true;
}

void gui_close(void)
{
    pthread_mutex_destroy(&gui_mutex);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}