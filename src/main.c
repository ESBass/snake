#include <stdio.h>
#include <SDL3/SDL.h>

#define HEIGHT 720
#define WIDTH 1280

#define GAME_X0 140
#define GAME_Y0 84

void DrawBackground(void);

struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} typedef gamestate;

struct {
    int x, y;
} typedef Vector2;

struct {
    int x, y;
} typedef snake;

gamestate state;

Vector2 WorldSpaceToScreenSpace(Vector2 coords);

int main(int argc, char** argv){

    //Used to check the window state.
    int shouldClose = 0;

    snake player;

    //Init sdl.
    if(!SDL_Init(SDL_INIT_VIDEO)){
        return -1;
    }

    //Create renderer and window.
    SDL_Window* window = SDL_CreateWindow("WINDOW", WIDTH, HEIGHT, 0);
    state.renderer = SDL_CreateRenderer(window, NULL);

    if(!window) return -1;

    player.x = 0;
    player.y = 0;

    while (!shouldClose) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_EVENT_QUIT:
                    shouldClose = 1;
                    break;
            }
        }

        DrawBackground();

        SDL_SetRenderDrawColor(state.renderer, 233, 0, 0, 255);
        Vector2 snakePos = WorldSpaceToScreenSpace((Vector2){player.x, player.y});
        SDL_RenderFillRect(state.renderer, &(SDL_FRect){snakePos.x, snakePos.y, 50, 45});

        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(window);
    return 0;
}

void DrawBackground(void)
{
    SDL_SetRenderDrawColor(state.renderer, 7, 0, 87, 255);
    SDL_RenderClear(state.renderer);
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(state.renderer, &(SDL_FRect){100, 56, 1080, 600});
    SDL_SetRenderDrawColor(state.renderer, 7, 0, 87, 255);
    //Play Area xy (140)(84) width/height(1000)(550)#

    int currentX = GAME_X0;
    int currentY = GAME_Y0;
    while(currentY < 600){
    while(currentX < 1100){
            SDL_RenderFillRect(state.renderer, &(SDL_FRect){currentX, currentY, 45, 45});
            currentX+=50;
        }
        currentY += 50;
        currentX = 140;
    }
}

Vector2 WorldSpaceToScreenSpace(Vector2 coords)
{
    int screenx = coords.x + GAME_X0;
    int screeny = coords.y + GAME_Y0;

    return (Vector2){screenx, screeny};
}
