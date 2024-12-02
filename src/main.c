#include <stdio.h>
#include <SDL3/SDL.h>

#define HEIGHT 720
#define WIDTH 1280

int main(int argc, char** argv){

    int shouldClose = 0;

    if(!SDL_Init(SDL_INIT_VIDEO)){
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("WINDOW", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    SDL_SetRenderDrawColor(renderer, 7, 0, 87, 255);

    if(!window) return -1;

    while (!shouldClose) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_EVENT_QUIT:
                    shouldClose = 1;
                    break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}