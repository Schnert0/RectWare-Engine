# RectWare Engine
The game engine used to create Slimemageddon

## Building and Dependancies
This code base requires SDL2 and OpenGL to build.

Build the project in the terminal using:
```
Make
./RectWare
```
This project was originally programed on MacOS.

## File Structure
- `assets`: images, music, etc. (not included in public repo)
- `src`: source code folder
  - `engine`: the game engine code itself
    - `core`: all the components that make up the game engine
  - `slimemageddon`: all code related to slimemageddon
    - `entities`: all entities that make up the game
  - `lib`: helper libraries (thank you Sean Barrett and co. <3)

## License
(For more info see [The License](LICENSE)) You are free to use this as the base for your own game. While you don't have to credit me, I would really appreciate it :)
