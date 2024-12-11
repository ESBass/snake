#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

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
    int paused, nopause;
    int score;
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
    int lastUpdateDir;
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

    //Starting variables for the snake
    int snakeStartLength = 3;
    int snakeInterval = 250;
    state.nopause = 0;

    state.score = 0;

    //Parameter processings
    for (int arg = 1; arg < argc; arg++)
    {
        char* param = argv[arg];

        if(!strcmp(param, "-snakelength")){
            snakeStartLength = atoi(argv[arg+1]);
        }

        if(!strcmp(param, "-snakespeed")){
            char* speed  = argv[arg+1];
            if(!strcmp(speed, "--veryslow")) snakeInterval = 1000;
            else if(!strcmp(speed, "--slow")) snakeInterval = 500;
            else if(!strcmp(speed, "--normal")) continue;
            else if(!strcmp(speed, "--fast")) snakeInterval = 125;
            else if(!strcmp(speed, "--veryfast")) snakeInterval = 62;
            else if(!strcmp(speed, "--suicide")) snakeInterval = 5;
            else snakeInterval = atoi(speed);
        }

        if(!strcmp(param, "-nopause")) state.nopause = 1;

        if(!strcmp(param, "-snakescore")){
            state.score = atoi(argv[arg+1]);
        }

        if(!strcmp(param, "-closeondie")){
            continue;
        }
    }
    

    //Used to check the window state.
    int shouldClose = 0;


    //Create player snake.
    snake player;
    Vector2 apple = {0};
    srand(time(NULL));

    player.length = snakeStartLength;

    //Array for all the snake tail segment positions, initialised to -1.
    Vector2 snakeTail [220] = {-1}; 

    //Init sdl.
    if(!SDL_Init(SDL_INIT_VIDEO)){
        printf("%s\n", "SDL Video Subsystem failed to initialise:");
        printf("%s\n", SDL_GetError());
        return -1;
    }

    if(!TTF_Init()){
        printf("%s\n", "SDL TTF Subsystem failed to initialise:");
        printf("%s\n", SDL_GetError());
        return -1;
    }

    TTF_Font *font;

    font = TTF_OpenFont("../res/Score_font.ttf", 24);

    if(!font){
        printf("%s\n", "Font error");
        printf("%s\n", SDL_GetError());
        return -1;
    }


    //Create renderer and window.
    state.window = SDL_CreateWindow("Snake - Liz Bass", WIDTH, HEIGHT, 0);
    state.renderer = SDL_CreateRenderer(state.window, NULL);

    //If one of wasn't created, return out.
    if(!state.window) return -1;
    if(!state.renderer) return -1;

    //Set player pos to 0,0
    player.x = 0;
    player.y = 0;

    player.direction = 0;
    player.lastUpdateDir = player.direction;

    //Used for delta time.
    float lastupdate = 0;

    MoveApple(&apple, snakeTail, &player);

    SDL_Color white = {255, 255, 255, 255};
    char string[100], outbuff[125];

    sprintf(string, "%d", state.score);

    strcat(outbuff, "Score: ");
    strcat(outbuff, string);

    SDL_Surface* scoreboard = TTF_RenderText_Solid(font, outbuff, 0, white);

    if(scoreboard == NULL){
        printf("%s\n", "Could not render text");
        printf("%s\n", SDL_GetError());
        return -1;
    }    

    SDL_Texture* text_texture;
    SDL_FRect dest = {((WIDTH/2) - (scoreboard->w / 2)), 10, scoreboard->w, scoreboard->h};
    state.paused = 0;

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
        if(SDL_GetTicks() - lastupdate > snakeInterval){
            if(state.paused) goto render;
            MoveSnake(&player);
            player.lastUpdateDir = player.direction;
            MoveTail(player, snakeTail);

            Vector2 playerapplepos = WorldSpaceToScreenSpace((Vector2){player.x, player.y});

            if(playerapplepos.x == apple.x && playerapplepos.y == apple.y){
                player.length++;
                state.score++;
                MoveApple(&apple, snakeTail, &player);

                memset(string, 0, strlen(string));
                memset(outbuff, 0, strlen(outbuff));

                sprintf(string, "%d", state.score);

                strcat(outbuff, "Score: ");
                strcat(outbuff, string);

                scoreboard = TTF_RenderText_Solid(font, outbuff, 0, white);
                text_texture = SDL_CreateTextureFromSurface(state.renderer, scoreboard);
                dest = (SDL_FRect){((WIDTH/2) - (scoreboard->w / 2)), 10, scoreboard->w, scoreboard->h};
            }

            lastupdate = SDL_GetTicks();
        }

render:
        DrawBackground();

        RenderSnake(player, snakeTail);

        SDL_SetRenderDrawColor(state.renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(state.renderer, &(SDL_FRect){apple.x, apple.y, 45, 45});


        SDL_RenderTexture(state.renderer, text_texture, &(SDL_FRect){0, 0, scoreboard->w, scoreboard->h}, &dest);
        SDL_RenderPresent(state.renderer);

        
    }

    SDL_DestroyTexture(text_texture);
    SDL_DestroySurface(scoreboard);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    TTF_Quit();
    SDL_Quit();
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
        if(s->lastUpdateDir == DIR_DOWN || state.paused) break;
        s->direction = DIR_UP;
        break;
    case SDLK_DOWN:
        if(s->lastUpdateDir == DIR_UP || state.paused) break;
        s->direction = DIR_DOWN;
        break;

    case SDLK_LEFT:
        if(s->lastUpdateDir == DIR_RIGHT || state.paused) break;
        s->direction = DIR_LEFT;
        break;

    case SDLK_RIGHT:
        if(s->lastUpdateDir == DIR_LEFT || state.paused) break;
        s->direction = DIR_RIGHT;
        break;
    
    case SDLK_ESCAPE:
        if(!state.nopause) state.paused = !state.paused;
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