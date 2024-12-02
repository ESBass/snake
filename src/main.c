#include <stdio.h>
#include <SDL3/SDL.h>

#define HEIGHT 720
#define WIDTH 1280

#define GAME_X0 140
#define GAME_Y0 84

#define DIR_RIGHT 0
#define DIR_LEFT 1
#define DIR_UP 2
#define DIR_DOWN 3

#define STEP_SIZE 50

struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} typedef gamestate;

struct {
    int x, y;
} typedef Vector2;

struct {
    int x, y;
    int direction;
    int length;
} typedef snake;

gamestate state;

Vector2 WorldSpaceToScreenSpace(Vector2 coords);
void DrawBackground(void);
void MoveSnake(snake* s);
void HandleSnakeInput(snake* s, SDL_Event* event);

void MoveTail(snake s, Vector2 tail[]){
    Vector2 prev = {0};
    Vector2 current = (Vector2){s.x, s.y};
    Vector2 head = current;

    for(int i = 0; i < 220; i++){
        prev = tail[i];
        if(prev.x == head.x && prev.y == head.y && i < s.length){
            printf("%s\n", "Game Over");
        }

        tail[i] = current;
        current = prev;
    }
}

void RenderSnake(snake s, Vector2 tail[]);

int main(int argc, char** argv){

    //Used to check the window state.
    int shouldClose = 0;

    snake player;

    player.length = 5;

    Vector2 snakeTail [220] = {-1}; 

    //Init sdl.
    if(!SDL_Init(SDL_INIT_VIDEO)){
        return -1;
    }

    //Create renderer and window.
    state.window = SDL_CreateWindow("WINDOW", WIDTH, HEIGHT, 0);
    state.renderer = SDL_CreateRenderer(state.window, NULL);

    if(!state.window) return -1;

    player.x = 0;
    player.y = 0;

    player.direction = 0;

    float lastupdate = 0;

    while (!shouldClose) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_EVENT_QUIT:
                    shouldClose = 1;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    HandleSnakeInput(&player, &ev);
            }   
        }

        if(SDL_GetTicks() - lastupdate > 500){
            MoveSnake(&player);
            MoveTail(player, snakeTail);
            lastupdate = SDL_GetTicks();
        }

        DrawBackground();

        RenderSnake(player, snakeTail);

        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
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

void MoveSnake(snake *s)
{
    switch (s->direction)
    {
    case DIR_RIGHT:
        s->x += STEP_SIZE;
        break;
    case DIR_LEFT:
        s->x -= STEP_SIZE;
        break;
    case DIR_UP:
        s->y -= STEP_SIZE;
        break;
    case DIR_DOWN:
        s->y += STEP_SIZE;
        break;
    default:
        break;
    }
}

void HandleSnakeInput(snake *s, SDL_Event* event)
{
    switch (event->key.key)
    {
    case SDLK_UP:
        if(s->direction == DIR_DOWN) break;
        s->direction = DIR_UP;
        printf("%s", "UP KEY");
        break;
    case SDLK_DOWN:
        if(s->direction == DIR_UP) break;
        s->direction = DIR_DOWN;
        printf("%s", "DOWN KEY");
        break;

    case SDLK_LEFT:
        if(s->direction == DIR_RIGHT) break;
        s->direction = DIR_LEFT;
        printf("%s", "KEY LEFT");
        break;

    case SDLK_RIGHT:
        if(s->direction == DIR_LEFT) break;
        s->direction = DIR_RIGHT;
        printf("%s", "RIGHT KEY");
        break;
    
    default:
        break;
    }
}

void RenderSnake(snake s, Vector2 tail[])
{
    // Draw head
    SDL_SetRenderDrawColor(state.renderer, 233, 0, 0, 255);
    Vector2 snakePos = WorldSpaceToScreenSpace((Vector2){s.x, s.y});
    SDL_RenderFillRect(state.renderer, &(SDL_FRect){snakePos.x, snakePos.y, 45, 45});

    //Render tail

    for(int seg = 0; seg < s.length; seg++){
        Vector2 segCoords = WorldSpaceToScreenSpace(tail[seg]);
        SDL_RenderFillRect(state.renderer, &(SDL_FRect){segCoords.x, segCoords.y, 45, 45});
    }
}

Vector2 WorldSpaceToScreenSpace(Vector2 coords)
{
    int screenx = coords.x + GAME_X0;
    int screeny = coords.y + GAME_Y0;

    return (Vector2){screenx, screeny};
}
