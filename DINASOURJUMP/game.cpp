#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 1536 * 2 / 3;
const int SCREEN_HEIGHT = 1024 * 2 / 3;
const int GROUND_LEVEL = 500;
const int INITIAL_DINO_WIDTH = 120;
const int INITIAL_DINO_HEIGHT = 120;
const float GRAVITY = 1;
const float JUMP_FORCE = 25;
const float FLAPPY_GRAVITY = 1;
const float FLAPPY_JUMP_FORCE = 15;
const float OBSTACLE_SPEED = 8.0f;
const int FRAME_DELAY = 1000 / 120;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* dinoTextures[3];
SDL_Texture* obstacleTexture = nullptr;
SDL_Texture* flappyObstacleTexture = nullptr;
SDL_Texture* gameOverTexture = nullptr;
SDL_Texture* youWinTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* growItemTexture = nullptr;
SDL_Texture* shrinkItemTexture = nullptr;
TTF_Font* font = nullptr;

bool isRunning = true;
bool isJumping = false;
bool gameOver = false;
bool gameWon = false;
bool isFlappyMode = false;
int obstacleCount = 0;

float dinoY = static_cast<float>(GROUND_LEVEL);
float velocityY = 0.0f;
int frameCount = 0;
int currentDinoFrame = 0;
int dinoFrameTimer = 0;
const int DINO_ANIMATION_SPEED = 10;
float dinoWidth = INITIAL_DINO_WIDTH;
float dinoHeight = INITIAL_DINO_HEIGHT;

struct Obstacle {
    float x;
    int width, height;
    bool passed = false;
    int gapY;
    int gapHeight = 300;
};
std::vector<Obstacle> obstacles;

struct Item {
    float x;
    int y;
    int width = 40;
    int height = 40;
    bool isGrow;
    bool collected = false;
};
std::vector<Item> items;

SDL_Texture* loadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void spawnObstacle() {
    if (!isFlappyMode) {
        int width = (35 + rand() % 20) * 2;
        int height = (60 + rand() % 30) * 2;
        obstacles.push_back({static_cast<float>(SCREEN_WIDTH), width, height, false, 0});
    } else {
        int width = 50 * 2;
        int gapY = 100 + rand() % (SCREEN_HEIGHT - 400);
        obstacles.push_back({static_cast<float>(SCREEN_WIDTH), width, SCREEN_HEIGHT, false, gapY});
    }
}

void spawnItem() {
    int y = 400 ;
    bool isGrow = rand() % 2 == 0;
    float itemX = static_cast<float>(SCREEN_WIDTH);
    if (obstacles.size() >= 2) {
        // Lấy hai chướng ngại vật cuối cùng
        auto& obs1 = obstacles[obstacles.size() - 2]; // Chướng ngại vật trước
        auto& obs2 = obstacles[obstacles.size() - 1]; // Chướng ngại vật sau
        itemX = (obs1.x + obs1.width + obs2.x) / 2.0f; // Tọa độ x ở giữa
    }

    items.push_back({itemX, y, 40, 40, isGrow});
}

void restartGame() {
    gameOver = false;
    gameWon = false;
    dinoY = static_cast<float>(GROUND_LEVEL);
    velocityY = 0.0f;
    obstacles.clear();
    items.clear();
    frameCount = 0;
    obstacleCount = 0;
    isFlappyMode = false;
    dinoWidth = INITIAL_DINO_WIDTH;
    dinoHeight = INITIAL_DINO_HEIGHT;
}

void handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        if (!gameOver && !gameWon && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
            if (!isFlappyMode && dinoY >= GROUND_LEVEL - 1) {
                isJumping = true;
                velocityY = -JUMP_FORCE;
            } else if (isFlappyMode) {
                velocityY = -FLAPPY_JUMP_FORCE;
            }
        }
        if ((gameOver || gameWon) && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
            restartGame();
        }
    }
}

void update(float deltaTime) {
    if (gameOver || gameWon) return;

    dinoY += velocityY * deltaTime * 60.0f;
    if (!isFlappyMode) {
        velocityY += GRAVITY * deltaTime * 60.0f;
        if (dinoY >= GROUND_LEVEL) {
            dinoY = GROUND_LEVEL;
            isJumping = false;
            velocityY = 0.0f;
        }
    } else {
        velocityY += FLAPPY_GRAVITY * deltaTime * 60.0f;
        if (dinoY < 0 || dinoY > SCREEN_HEIGHT) gameOver = true;
    }

    for (auto& obs : obstacles) obs.x -= OBSTACLE_SPEED * deltaTime * 60.0f;
    if (!obstacles.empty() && obstacles[0].x < -obstacles[0].width) obstacles.erase(obstacles.begin());

    for (auto& item : items) item.x -= OBSTACLE_SPEED * deltaTime * 60.0f;
    if (!items.empty() && items[0].x < -items[0].width) items.erase(items.begin());

    for (auto& obs : obstacles) {
        if (!obs.passed && obs.x + obs.width < 100) {
            obs.passed = true;
            obstacleCount++;
            if (!isFlappyMode && obstacleCount % 3 == 0) {
                spawnItem();
            }
            if (!isFlappyMode && obstacleCount == 5) {
                isFlappyMode = true;
                dinoY = SCREEN_HEIGHT / 2.0f;
                velocityY = 0.0f;
                obstacles.clear();
                items.clear();
            } else if (isFlappyMode && obstacleCount == 10) {
                gameWon = true;
            }
        }
    }

    if (frameCount % (isFlappyMode ? 180 : 120) == 0 && !isFlappyMode) spawnObstacle();
    if (isFlappyMode && frameCount % 180 == 0 && obstacleCount < 10) spawnObstacle();

    for (auto& obs : obstacles) {
        int dinoLeft = 100;
        int dinoRight = 100 + static_cast<int>(dinoWidth);
        int dinoTop = static_cast<int>(dinoY) - static_cast<int>(dinoHeight);
        int dinoBottom = static_cast<int>(dinoY);

        if (!isFlappyMode) {
            int obsLeft = static_cast<int>(obs.x);
            int obsRight = obsLeft + obs.width;
            int obsTop = GROUND_LEVEL - obs.height;
            int obsBottom = GROUND_LEVEL;

            if (dinoRight > obsLeft && dinoLeft < obsRight && dinoBottom > obsTop && dinoTop < obsBottom) {
                gameOver = true;
            }
        } else {
            int topPipeBottom = obs.gapY;
            int bottomPipeTop = obs.gapY + obs.gapHeight;
            int obsLeft = static_cast<int>(obs.x);
            int obsRight = obsLeft + obs.width;

            if (dinoRight > obsLeft && dinoLeft < obsRight && (dinoTop < topPipeBottom || dinoBottom > bottomPipeTop)) {
                gameOver = true;
            }
        }
    }

    for (auto& item : items) {
        if (item.collected) continue;

        int dinoLeft = 100;
        int dinoRight = 100 + static_cast<int>(dinoWidth);
        int dinoTop = static_cast<int>(dinoY) - static_cast<int>(dinoHeight);
        int dinoBottom = static_cast<int>(dinoY);

        int itemLeft = static_cast<int>(item.x);
        int itemRight = itemLeft + item.width;
        int itemTop = item.y;
        int itemBottom = item.y + item.height;

        if (dinoRight > itemLeft && dinoLeft < itemRight && dinoBottom > itemTop && dinoTop < itemBottom) {
            item.collected = true;
            if (item.isGrow) {
                dinoWidth *= 1.2f;
                dinoHeight *= 1.2f;
            } else {
                dinoWidth /= 1.2f;
                dinoHeight /= 1.2f;
            }
        }
    }

    dinoFrameTimer++;
    if (dinoFrameTimer >= DINO_ANIMATION_SPEED) {
        currentDinoFrame = (currentDinoFrame + 1) % 3;
        dinoFrameTimer = 0;
    }

    frameCount++;
}

void renderText(const std::string& text, int x, int y) {
    SDL_Color color = {255, 0, 0, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

    if (!gameOver && !gameWon) {
        SDL_Rect dinoRect = {100, static_cast<int>(dinoY) - static_cast<int>(dinoHeight),
                             static_cast<int>(dinoWidth), static_cast<int>(dinoHeight)};
        SDL_RenderCopy(renderer, dinoTextures[currentDinoFrame], NULL, &dinoRect);

        for (auto& obs : obstacles) {
            if (!isFlappyMode) {
                SDL_Rect obsRect = {static_cast<int>(obs.x), GROUND_LEVEL - obs.height, obs.width, obs.height};
                SDL_RenderCopy(renderer, obstacleTexture, NULL, &obsRect);
            } else {
                SDL_Rect topPipe = {static_cast<int>(obs.x), 0, obs.width, obs.gapY};
                SDL_RenderCopy(renderer, flappyObstacleTexture, NULL, &topPipe);
                SDL_Rect bottomPipe = {static_cast<int>(obs.x), obs.gapY + obs.gapHeight,
                                       obs.width, SCREEN_HEIGHT - (obs.gapY + obs.gapHeight)};
                SDL_RenderCopy(renderer, flappyObstacleTexture, NULL, &bottomPipe);
            }
        }

        for (auto& item : items) {
            if (!item.collected) {
                SDL_Rect itemRect = {static_cast<int>(item.x) + 50, item.y + 50, item.width + 50, item.height +50};
                SDL_RenderCopy(renderer, item.isGrow ? growItemTexture : shrinkItemTexture, NULL, &itemRect);
            }
        }
    }

    renderText("Obstacles: " + std::to_string(obstacleCount), 10, 10);

    if (gameOver) {
        SDL_Rect gameOverRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 100};
        SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
        renderText("Press Enter to Restart", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60);
    } else if (gameWon) {
        SDL_Rect youWinRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 100};
        SDL_RenderCopy(renderer, youWinTexture, NULL, &youWinRect);
        renderText("Press Enter to Restart", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60);
    }

    SDL_RenderPresent(renderer);
}

void clean() {
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(dinoTextures[i]);
    }
    SDL_DestroyTexture(obstacleTexture);
    SDL_DestroyTexture(flappyObstacleTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(youWinTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(growItemTexture);
    SDL_DestroyTexture(shrinkItemTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (IMG_Init(IMG_INIT_PNG) < 0) return 1;
    if (TTF_Init() == -1) return 1;

    window = SDL_CreateWindow("Dino Run", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    dinoTextures[0] = loadTexture("C:\\DINASOURJUMP\\dina1.png");
    dinoTextures[1] = loadTexture("C:\\DINASOURJUMP\\dina2.png");
    dinoTextures[2] = loadTexture("C:\\DINASOURJUMP\\dina3.png");
    if (!dinoTextures[0] || !dinoTextures[1] || !dinoTextures[2]) return 1;

    obstacleTexture = loadTexture("C:\\DINASOURJUMP\\tree.png");
    if (!obstacleTexture) return 1;

    flappyObstacleTexture = loadTexture("C:\\DINASOURJUMP\\rock1.png");
    if (!flappyObstacleTexture) return 1;

    gameOverTexture = loadTexture("C:\\DINASOURJUMP\\gameover.png");
    if (!gameOverTexture) return 1;

    youWinTexture = loadTexture("C:\\DINASOURJUMP\\youwin.png");
    if (!youWinTexture) return 1;

    backgroundTexture = loadTexture("C:\\DINASOURJUMP\\bg.png");
    if (!backgroundTexture) return 1;

    growItemTexture = loadTexture("C:\\DINASOURJUMP\\grow.png");
    if (!growItemTexture) return 1;

    shrinkItemTexture = loadTexture("C:\\DINASOURJUMP\\shrink.png");
    if (!shrinkItemTexture) return 1;

    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }

    Uint32 frameStart, previousFrame = SDL_GetTicks();
    float deltaTime;

    while (isRunning) {
        frameStart = SDL_GetTicks();
        deltaTime = (frameStart - previousFrame) / 1000.0f;
        previousFrame = frameStart;

        handleEvents();
        update(deltaTime);
        render();

        int frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    clean();
    return 0;
}
