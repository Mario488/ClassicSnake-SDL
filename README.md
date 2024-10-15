Snake Game using SDL in C++
This project is a classic implementation of the Snake game using C++ and the SDL (Simple DirectMedia Layer) library. The snake moves around the screen, consumes food, and grows longer while avoiding collisions with walls and itself. The game is rendered using SDL for graphics, and the score is displayed on the screen using SDL_ttf for text rendering.

Features
-Classic Snake gameplay: Control the snake with the arrow keys to consume food and grow longer.
-Dynamic Difficulty: The snake’s speed increases as it grows longer.
-Simple 2D graphics: Rendered using the SDL library.
-Score Tracking: Displays the current score in real-time using a custom font.
-Collision Detection: Snake collides with walls and itself, ending the game.

Installation and Setup
To run this project locally, follow these steps:

Prerequisites
Make sure you have the following installed:

C++ compiler (like g++)
SDL2 library
SDL2_ttf library

Clone the repository

Build the project
g++ -Iinclude -Iinclude/sdl -Iinclude/headers -Llib -o Main src/main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf