#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;
const int GROUND_LEVEL = 300;
const int DINO_WIDTH = 60;
const int DINO_HEIGHT = 60;
const int GRAVITY = 1;
const int JUMP_FORCE = 18;
const int OBSTACLE_SPEED = 5;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool isRunning = true;
bool isJumping = false;
bool gameOver = false;

int dinoY = GROUND_LEVEL;
int velocityY = 0;
int frameCount = 0;

struct Obstacle {
    int x, width, height;
};
std::vector<Obstacle> obstacles;

void spawnObstacle() {
    int width = 20 + rand() % 30;
    int height = 75;
    obstacles.push_back({SCREEN_WIDTH, width, height});
}

void restartGame() {
    gameOver = false;
    dinoY = GROUND_LEVEL;
    velocityY = 0;
    obstacles.clear();  // Xóa tất cả chướng ngại vật
    frameCount = 0;
}

void handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        if (!gameOver && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE && dinoY == GROUND_LEVEL) {
            isJumping = true;
            velocityY = -JUMP_FORCE;
        }
        if (gameOver && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
2            restartGame();
        }
    }

}

void update() {
    if (gameOver) return;

    if (isJumping) {
        dinoY += velocityY;
        velocityY += GRAVITY;
        if (dinoY >= GROUND_LEVEL) {
            dinoY = GROUND_LEVEL;
            isJumping = false;
        }
    }

    for (auto& obs : obstacles) obs.x -= OBSTACLE_SPEED;
    if (!obstacles.empty() && obstacles[0].x < -obstacles[0].width) obstacles.erase(obstacles.begin());

    if (frameCount % 100 == 0) spawnObstacle();

    // Kiểm tra va chạm
    for (auto& obs : obstacles) {
    // Xác định tọa độ của khủng long
    int dinoLeft = 100;
    int dinoRight = 100 + DINO_WIDTH;
    int dinoTop = dinoY;
    int dinoBottom = dinoY + DINO_HEIGHT;

    // Xác định tọa độ của chướng ngại vật
    int obsLeft = obs.x;
    int obsRight = obs.x + obs.width;
    int obsTop = GROUND_LEVEL - obs.height + 70; // Điều chỉnh đúng vị trí
    int obsBottom = obsTop + obs.height;

    // Kiểm tra va chạm hình chữ nhật (AABB Collision Detection)
    if (dinoRight > obsLeft && dinoLeft < obsRight && dinoBottom > obsTop && dinoTop < obsBottom) {
        gameOver = true;
    }
}


    frameCount++;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Vẽ khủng long
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect dinoRect = {100, dinoY, DINO_WIDTH, DINO_HEIGHT};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Vẽ chướng ngại vật
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (auto& obs : obstacles) {
        SDL_Rect obsRect = {obs.x, GROUND_LEVEL - obs.height + 70, obs.width, obs.height};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    if (gameOver) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect gameOverRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 30, 200, 60};
        SDL_RenderFillRect(renderer, &gameOverRect);
        std::cout << "Game Over! Press Enter to Restart" << std::endl;
    }

    SDL_RenderPresent(renderer);
}

void clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    window = SDL_CreateWindow("Dino Run", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }

    clean();
    return 0;
}
