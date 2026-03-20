#include "parkingGUI.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include <stdio.h>

#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 280
#define NUM_SPACES 3

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static TTF_Font     *font     = NULL;

static pthread_mutex_t gui_mutex = PTHREAD_MUTEX_INITIALIZER;

static int current_available = 0;
static int current_total = 0;
static int current_active = 0;
static int slot_car[NUM_SPACES] = {-1, -1, -1};
static bool simulation_finished = false;

static void draw_text(const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

bool gui_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return false;
    }

    if (TTF_Init() != 0) {
        SDL_Quit();
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
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    if (!font) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}

void gui_set_status(int available, int total, int active)
{
    pthread_mutex_lock(&gui_mutex);
    current_available = available;
    current_total = total;
    current_active = active;
    pthread_mutex_unlock(&gui_mutex);
}

void gui_set_slot_car(int slot, int car_id)
{
    pthread_mutex_lock(&gui_mutex);
    if (slot >= 0 && slot < NUM_SPACES) {
        slot_car[slot] = car_id;
    }
    pthread_mutex_unlock(&gui_mutex);
}

void gui_set_finished(bool finished)
{
    pthread_mutex_lock(&gui_mutex);
    simulation_finished = finished;
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
    int active = current_active;
    bool finished = simulation_finished;

    int local_slot_car[NUM_SPACES];
    for (int i = 0; i < NUM_SPACES; i++) {
        local_slot_car[i] = slot_car[i];
    }
    pthread_mutex_unlock(&gui_mutex);

    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderClear(renderer);

    SDL_Color black = {0, 0, 0, 255};

    char info[64];

    snprintf(info, sizeof(info), "Available spaces: %d", available);
    draw_text(info, 40, 30, black);

    snprintf(info, sizeof(info), "Active cars: %d", active);
    draw_text(info, 40, 70, black);

    for (int i = 0; i < total; i++) {
        SDL_Rect box = {60 + i * 180, 140, 100, 100};

        if (local_slot_car[i] == -1) {
            SDL_SetRenderDrawColor(renderer, 60, 180, 75, 255);   // verde
        } else {
            SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);   // rojo
        }

        SDL_RenderFillRect(renderer, &box);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &box);

        if (local_slot_car[i] != -1) {
            char car_label[32];
            snprintf(car_label, sizeof(car_label), "Car %d", local_slot_car[i]);
            draw_text(car_label, box.x + 10, box.y + 35, black);
        }
    }

    if (finished) {
        SDL_Color red = {200, 0, 0, 255};
        draw_text("Simulation finished", 220, 90, red);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
    return true;
}

void gui_close(void)
{
    pthread_mutex_destroy(&gui_mutex);

    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}