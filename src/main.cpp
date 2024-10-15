#include <iostream>
#include <SDL.h>
#include <vector>
#include <random>
#include <SDL_ttf.h>
#include <ctime>
#include <string>
using namespace std;
// constants
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define INITIAL_SNAKE_SIZE 4
#define MAX_FPS 60
#define FRAME_DELAY (1000 / MAX_FPS)
#define SNAKE_SIZE 35
#define BORDER_THICKNESS SNAKE_SIZE
mt19937 rng(time(0));
Uint16 SCORE = 0;
// Defining variables for convenience
const int borderTopLeftY = (int)((SCREEN_HEIGHT * 0.1) / SNAKE_SIZE) * SNAKE_SIZE;
const int borderTopBottomWidth = (int)(SCREEN_WIDTH / SNAKE_SIZE) * SNAKE_SIZE;
const int borderBottomY = (int)((SCREEN_HEIGHT - BORDER_THICKNESS) / SNAKE_SIZE) * SNAKE_SIZE;
const int borderLeftRightHeight = (int)((SCREEN_HEIGHT * 0.9) / SNAKE_SIZE) * SNAKE_SIZE;
const int borderRightX = (int)((SCREEN_WIDTH - BORDER_THICKNESS) / SNAKE_SIZE) * SNAKE_SIZE;
const int borderRightY = (int)(SCREEN_HEIGHT * 0.1);
enum DIRS
{
    UP = 1,
    DOWN,
    LEFT,
    RIGHT
};
// updating snake position
void updateSnake(vector<SDL_Rect> &snake, DIRS dir)
{
    // save the snake's head in order to simulate movement
    SDL_Rect prevHead = snake[0];
    switch (dir)
    {
    case UP:
        // if the snake is moving up, decrease it's y coordinate of the head
        // by the size of one segment of the snake
        snake[0].y -= SNAKE_SIZE;
        break;
    case DOWN:
        snake[0].y += SNAKE_SIZE;
        break;
    case LEFT:
        snake[0].x -= SNAKE_SIZE;
        break;
    case RIGHT:
        snake[0].x += SNAKE_SIZE;
        break;
    }
    // Moving the snake
    for (int i = 1; i < snake.size(); i++)
    {
        // save the current body part in temporary variable
        SDL_Rect temp = snake[i];
        // Move the current segment to the position of the previous head
        // this "shifts" the segment to follow the part in front of it
        snake[i] = prevHead;
        // "shifting" the head
        prevHead = temp;
    }
}
// Generating random number in range [left, right]
int getRandomNumInRange(int left, int right)
{
    // Ensuring that left and right boundaries is divisible by snake's size
    // so that food items spawn aligned with the grid
    if (left % SNAKE_SIZE != 0)
        left += SNAKE_SIZE - (left % SNAKE_SIZE);
    if (right % SNAKE_SIZE != 0)
        right -= right % SNAKE_SIZE;
    // generates random number between the given boundaries
    uniform_int_distribution<int> dist(left / SNAKE_SIZE, right / SNAKE_SIZE);
    // Ensuring it is a multiple of the snake's size
    return dist(rng) * SNAKE_SIZE;
}
// Checks if food item coordinates collide with the snake's body
void checkFoodCoordinate(const vector<SDL_Rect> &snake, int &x, int &y)
{
    bool found;
    do
    {
        // Generate new random coordinates for food
        x = getRandomNumInRange(BORDER_THICKNESS, ((int)(SCREEN_WIDTH / SNAKE_SIZE) * SNAKE_SIZE) - 2 * BORDER_THICKNESS);
        y = getRandomNumInRange(((int)((SCREEN_HEIGHT * 0.1) / SNAKE_SIZE) * SNAKE_SIZE) + BORDER_THICKNESS,
                                ((int)((SCREEN_HEIGHT - BORDER_THICKNESS) / SNAKE_SIZE) * SNAKE_SIZE) - BORDER_THICKNESS);
        // Check if new coordinates overlap with any part of the snake
        found = false;
        for (int i = 0; i < snake.size(); i++)
        {
            if (snake[i].x == x && snake[i].y == y)
            {
                found = true;
                break;
            }
        }
    } while (found); // Repeat until new coordinates don't overlap with the snake
}
SDL_Rect createFoodItem(const vector<SDL_Rect> &snake)
{
    // Generate coordinates for the food item
    int food_x, food_y;
    // Validate food coordinates
    checkFoodCoordinate(snake, food_x, food_y);
    return SDL_Rect{food_x, food_y, SNAKE_SIZE, SNAKE_SIZE};
}
bool checkSelfCollision(vector<SDL_Rect> &snake)
{
    // Get the head of the snake
    SDL_Rect head = snake[0];
    for (int i = 1; i < snake.size(); i++)
    {
        // Check if the head position matches any other segment of the snake
        if (head.x == snake[i].x && head.y == snake[i].y)
        {
            return true;
        }
    }
    return false;
}
void isFoodConsumed(vector<SDL_Rect> &snake, SDL_Rect &food, DIRS dir, float &moveInterval)
{
    // Checks if the snake's head matches the food item's coordinates
    if (snake[0].x == food.x && snake[0].y == food.y)
    {
        // Creating new segment since we confirmed that item is eaten
        SDL_Rect lastSegment = snake[snake.size() - 1];
        int newX = 0, newY = 0;
        // Setting new segment's coordinates depending on snake's direction
        switch (dir)
        {
        case LEFT:
            newX = lastSegment.x - SNAKE_SIZE;
            newY = lastSegment.y;
            break;
        case RIGHT:
            newX = lastSegment.x + SNAKE_SIZE;
            newY = lastSegment.y;
            break;
        case UP:
            newX = lastSegment.x;
            newY = lastSegment.y - SNAKE_SIZE;
            break;
        case DOWN:
            newX = lastSegment.x;
            newY = lastSegment.y + SNAKE_SIZE;
            break;
        }
        // Appending new segment at the back of the snake
        snake.push_back(SDL_Rect{newX, newY, SNAKE_SIZE, SNAKE_SIZE});
        // Decrease the moveInterval (speed up) the snake as it gets bigger
        moveInterval -= 0.1;
        // Generate new food item
        food = createFoodItem(snake);
        // increase game score
        SCORE++;
    }
}
void SnakeRendering(SDL_Renderer *renderer, vector<SDL_Rect> &snake, bool &run)
{
    for (int i = 0; i < snake.size(); i++)
    {
        // Different color for the snake's head
        if (i == 0)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &snake[i]);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 99, 71, 255);
            SDL_RenderFillRect(renderer, &snake[i]);
        }
    }
}
void BordersRendering(SDL_Renderer *renderer, SDL_Rect topBorder, SDL_Rect bottomBorder, SDL_Rect leftBorder, SDL_Rect rightBorder)
{
    // Border Color
    SDL_SetRenderDrawColor(renderer, 201, 187, 34, 0.593);
    // Rendering game borders
    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
}
// fill the snake's vector initially
void populateSnake(vector<SDL_Rect> &snake)
{
    for (int i = 0; i < INITIAL_SNAKE_SIZE; i++)
    {
        SDL_Rect segment = {
            // Ensuring it aligns with the grid
            ((SCREEN_WIDTH / 2) - ((SCREEN_WIDTH / 2) % SNAKE_SIZE)) - (i * SNAKE_SIZE),
            (SCREEN_HEIGHT / 2) - ((SCREEN_HEIGHT / 2) % SNAKE_SIZE),
            SNAKE_SIZE, SNAKE_SIZE};
        snake.push_back(segment);
    }
}
bool checkBorderCollision(SDL_Rect head, SDL_Rect topBorder, SDL_Rect bottomBorder, SDL_Rect leftBorder, SDL_Rect rightBorder)
{
    if (head.y == topBorder.y)
    {
        return true;
    }
    else if (head.y == bottomBorder.y)
    {
        return true;
    }
    else if (head.x == leftBorder.x)
    {
        return true;
    }
    else if (head.x == rightBorder.x)
    {
        return true;
    }
    return false;
}
// void DisplayScore
void handleSnakeMovement(Uint32 currentTime, Uint32 &lastMoveTime, float &moveInterval, DIRS &dir, DIRS nextDir, vector<SDL_Rect> &snake, SDL_Rect &food, bool &run, SDL_Rect topBorder, SDL_Rect bottomBorder, SDL_Rect leftBorder, SDL_Rect rightBorder)
{
    if (currentTime - lastMoveTime >= moveInterval)
    {
        // Apply the next direction before moving the snake
        dir = nextDir;
        // checks for collision with food item
        isFoodConsumed(snake, food, dir, moveInterval);
        // Border collision
        if (checkBorderCollision(snake[0], topBorder, bottomBorder, leftBorder, rightBorder))
        {
            // end immediately
            run = false;
            return;
        }
        // checks for self collision
        updateSnake(snake, dir);
        if (checkSelfCollision(snake))
        {
            run = false;
        }
        lastMoveTime = currentTime;
    }
}

void renderText(SDL_Renderer *renderer, string text, TTF_Font *font, SDL_Color color, int x, int y)
{
    // Render text to an SDL surface
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);

    // Convert surface to texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); // No longer needed after creating the texture

    // Get texture width and height
    int textWidth = 0, textHeight = 0;
    SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);

    // Set the destination rectangle where the texture will be drawn
    SDL_Rect renderQuad = {x, y, textWidth, textHeight};

    // Render the texture
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);

    // Free the texture after rendering
    SDL_DestroyTexture(texture);
}
int main(int argc, char *argv[])
{
    // Initialising the game window pointer to NULL
    // which will later store the reference to the game window
    SDL_Window *window = NULL;
    // Initialising the game renderer pointer to NULL
    // which will later be used to render graphics on the game window
    SDL_Renderer *renderer = NULL;
    // Declaring event to handle input events
    SDL_Event e;
    // Initialise the SDL library
    SDL_Init(SDL_INIT_EVERYTHING);
    // Initialise TTF library
    TTF_Init();
    // Loading a font
    TTF_Font *font = TTF_OpenFont("./fonts/Baloo-Regular.ttf", (int)(borderTopLeftY / 1.3));
    SDL_Color textColor = {255, 255, 255};
    if (!font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return -1;
    }

    // Create an SDL window and renderer with specified dimensions
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    // Initialise a vector to hold the segments of the snake
    vector<SDL_Rect> snake{};
    // Populate with initial segments
    populateSnake(snake);
    bool run = true;
    // Initial snake direction
    DIRS dir = RIGHT;
    DIRS nextDir = RIGHT;
    // Save the time for last Move
    Uint32 lastMoveTime = SDL_GetTicks();
    // Refresh time(in ms) for the snake

    // As the grid gets smaller (hence the snake) the snake will become faster
    float moveInterval = 100 + (SNAKE_SIZE - 25) * 2;
    // Initialising all 4 borders
    SDL_Rect topBorder{0, borderTopLeftY, borderTopBottomWidth, BORDER_THICKNESS};
    SDL_Rect bottomBorder{0, borderBottomY, borderTopBottomWidth, BORDER_THICKNESS};
    SDL_Rect leftBorder{0, borderTopLeftY, BORDER_THICKNESS, borderLeftRightHeight};
    SDL_Rect rightBorder{borderRightX, borderRightY, BORDER_THICKNESS, borderLeftRightHeight};
    // Initialising the food item
    SDL_Rect food = createFoodItem(snake);
    // Game loop
    while (run)
    {
        // Get the current time(in ms) to track the start of the frame
        Uint32 frameStart = SDL_GetTicks();
        // handling user input
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    if (dir != DOWN)
                    {
                        nextDir = UP;
                    }
                    break;
                case SDLK_DOWN:
                    if (dir != UP)
                    {
                        nextDir = DOWN;
                    }
                    break;
                case SDLK_LEFT:
                    if (dir != RIGHT)
                    {
                        nextDir = LEFT;
                    }
                    break;
                case SDLK_RIGHT:
                    if (dir != LEFT)
                    {
                        nextDir = RIGHT;
                    }
                    break;
                }
            }
        }
        // Get the current time in order to control the snake's movement
        Uint32 currentTime = SDL_GetTicks();

        handleSnakeMovement(currentTime, lastMoveTime, moveInterval, dir, nextDir, snake, food, run, topBorder, bottomBorder, leftBorder, rightBorder);
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        string score_display = "Score: " + to_string(SCORE);
        renderText(renderer, score_display, font, textColor, 0, 0);

        // Game borders
        BordersRendering(renderer, topBorder, bottomBorder, leftBorder, rightBorder);
        // Rendering the snake
        SnakeRendering(renderer, snake, run);
        // Rendering the food with different color
        SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);
        SDL_RenderFillRect(renderer, &food);
        // Displaying the objects
        SDL_RenderPresent(renderer);
        // Ensuring the game runs at the specified FPS
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    // Clear up
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
