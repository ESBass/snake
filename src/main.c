#include <stdio.h>
#include <SDL3/SDL.h>
#include <time.h>
#include <stdlib.h>

//Define window width and height
#define HEIGHT 720
#define WIDTH 1280

//Define the offsets of the game area/
#define GAME_X0 140
#define GAME_Y0 84

//Define directions of the snake.
#define DIR_RIGHT 0
#define DIR_LEFT 1
#define DIR_UP 2
#define DIR_DOWN 3

//Define the difference between two squares.
#define STEP_SIZE 50

#define ROW_MAX 11
#define COLUMN_MAX 20

//Gamestate struct so the window and renderer can be quickly referenced.
struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} typedef gamestate;

//Basic vector 2 struct.
struct {
    int x, y;
} typedef Vector2;

//Define a snake
struct {
    int x, y; //Snake current position.
    int direction; //Direction the snake is heading in.
    int length; //The number of segments the snake has.
} typedef snake;

//Define a global "state" variable.
gamestate state;

// Convert world space to screen space coords.
static inline Vector2 WorldSpaceToScreenSpace(Vector2 coords);

//Draw the background and grid.
void DrawBackground(void);

//Move the snake.
void MoveSnake(snake* s);

void MoveApple(Vector2* applepos, Vector2 tail[], snake* s);

//Change snake direction.
void HandleSnakeInput(snake* s, SDL_Event* event);

void GameOver(){
    printf("\n%s\n", "Game Over!");
    exit(0);
}

//Basically a queue management function.
void MoveTail(snake s, Vector2 tail[]){
    Vector2 prev = {0};
    Vector2 current = (Vector2){s.x, s.y};
    Vector2 head = current;

    for(int i = 0; i < 220; i++){
        prev = tail[i];

        //This checks if the snake has collided with itself.
        //The s.length check is because the tail length array can hold every square in memory.
        if(prev.x == head.x && prev.y == head.y && i < s.length){
            GameOver();
        }

        tail[i] = current;
        current = prev;
    }
}

//Snake rendering function.
void RenderSnake(snake s, Vector2 tail[]);

int main(int argc, char** argv){

    //Used to check the window state.
    int shouldClose = 0;

    //Create player snake.
    snake player;
    Vector2 apple = {0};

    srand(time(NULL));

    player.length = 3;

    //Array for all the snake tail segment positions, initialised to -1.
    Vector2 snakeTail [220] = {-1}; 

    //Init sdl.
    if(!SDL_Init(SDL_INIT_VIDEO)){
        return -1;
    }

    //Create renderer and window.
    state.window = SDL_CreateWindow("WINDOW", WIDTH, HEIGHT, 0);
    state.renderer = SDL_CreateRenderer(state.window, NULL);

    //If one of wasn't created, return out.
    if(!state.window) return -1;
    if(!state.renderer) return -1;

    //Set player pos to 0,0
    player.x = 0;
    player.y = 0;

    player.direction = 0;

    //Used for delta time.
    float lastupdate = 0;

    MoveApple(&apple, snakeTail, &player);

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

        //Every 500ms move the snake.
        if(SDL_GetTicks() - lastupdate > 250){
            MoveSnake(&player);
            MoveTail(player, snakeTail);

            Vector2 playerapplepos = WorldSpaceToScreenSpace((Vector2){player.x, player.y});

            if(playerapplepos.x == apple.x && playerapplepos.y == apple.y){
                player.length++;
                MoveApple(&apple, snakeTail, &player);
            }

            lastupdate = SDL_GetTicks();
        }

        DrawBackground();

        RenderSnake(player, snakeTail);

        SDL_SetRenderDrawColor(state.renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(state.renderer, &(SDL_FRect){apple.x, apple.y, 45, 45});

        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
}

void DrawBackground(void)
{
    //Set the renderer to blue, and fill the screen.
    SDL_SetRenderDrawColor(state.renderer, 7, 0, 87, 255);
    SDL_RenderClear(state.renderer);

    //Set the renderer to white, and draw a rectangle in the middle of the screen.
    SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(state.renderer, &(SDL_FRect){100, 56, 1080, 600});

    //Change render colour back to blue.
    SDL_SetRenderDrawColor(state.renderer, 7, 0, 87, 255);

    //Play Area xy (140)(84) width/height(1000)(550)#

    //Draw a grid of blue squares in the play area.
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

    if(s->x > 951 || s->x < 0) GameOver();
    else if(s->y > 500 || s->y < 0) GameOver();
}

void HandleSnakeInput(snake *s, SDL_Event* event)
{

    //Each switch statements basically:
    //Checks that the move is valid
    //Does it.
    //Outputs to console
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

    Vector2 segCoords;
    Vector2 prevSegCoords = snakePos;

    for(int seg = 0; seg < s.length; seg++){

        int segWidth = 45;
        int segHeight = 45;
        int segxOffset = 0;
        int segyOffset = 0;

        segCoords = WorldSpaceToScreenSpace(tail[seg]);

        if(prevSegCoords.x > segCoords.x) segWidth = 50;
        if(prevSegCoords.y > segCoords.y) segHeight = 50;

        if(prevSegCoords.x < segCoords.x){
            segWidth = 50;
            segxOffset = -5;
        }

        if(prevSegCoords.y < segCoords.y){
            segHeight = 50;
            segyOffset = -5;
        }

        SDL_RenderFillRect(state.renderer, &(SDL_FRect){segCoords.x + segxOffset, segCoords.y + segyOffset, segWidth, segHeight});

        prevSegCoords = segCoords;
        
    }
}

void MoveApple(Vector2* applepos, Vector2 tail[], snake* s){
    int applex = rand() % COLUMN_MAX;
    int appley = rand() % ROW_MAX;

    applepos->x = (GAME_X0 + (applex * STEP_SIZE));
    applepos->y = (GAME_Y0 + (appley * STEP_SIZE));

    if((applepos->x == GAME_X0 && applepos->y == GAME_Y0)){
        MoveApple(applepos, tail, s);
    }

    for(int i = 0; i < s->length; i++){
        Vector2 segWorldPos = WorldSpaceToScreenSpace(tail[i]);
        if(segWorldPos.x == applepos->x && segWorldPos.y == applepos->y){
            MoveApple(applepos, tail, s);
        }
    }

    return;
}

static inline Vector2 WorldSpaceToScreenSpace(Vector2 coords)
{
    int screenx = coords.x + GAME_X0;
    int screeny = coords.y + GAME_Y0;

    return (Vector2){screenx, screeny};
}