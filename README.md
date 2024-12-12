This is just a test snake game.

## Command Line Parameters

`-snakelength x`: The number of segements the snake starts off with.

`-snakespeed x`: The speed the snake moves at. Can either be an integer, or one of the following.
- `--veryslow`
- `--slow`
- `--normal`
- `--fast`
- `--veryfast`
- `--suicide`

`-nopause`: Disables pausing.

`-snakescore x`: The starting score of the snake.

`-closeondie`: Closes the program on death instead of game over.

## Requirements

In order to run this, you will need to create a directory called `res` and inside place a ttf file called `Score_font.ttf`. This is basically to avoid dealing with font licensing issues.

__This has SDL_ttf as a dependency which can cause issues on Windows__

Attempt to compile on Windows at your own risk.
