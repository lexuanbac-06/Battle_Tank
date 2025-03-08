#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include<SDL_ttf.h>
#include<iostream>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<string>
#include<algorithm>
#undef main  

int score = 0;
int maxScore = 0; 
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
SDL_Texture* tankTexture = nullptr;
SDL_Texture* enemyTankTexture = nullptr;
SDL_Texture* wallTexture = nullptr;
SDL_Texture* bulletTexture = nullptr;
SDL_Texture* backgroundTexture2 = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* pauseButtonTexture = nullptr;
SDL_Texture* background_multitasking = nullptr;
SDL_Texture* lives1 = nullptr;
SDL_Texture* lives2 = nullptr;
SDL_Texture* lives3 = nullptr;

TTF_Font* font2 = nullptr;
TTF_Font* font3 = nullptr;
SDL_Rect pauseButtonRect = { 850, 30, 80, 80 }; // Kích thước và vị trí nút pause
SDL_Rect multitaskingRect = { 800, 0, 200, 600 };// Kích thước và vị trí phần đa nhiệm 
SDL_Rect Live = { 820, 100, 150, 300 };// Kích thước và vị trí phần mạng
bool isPaused = false;
bool running = true;
bool gameOver = 0;
bool inMenu = true;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
enum Direction { UP, DOWN, LEFT, RIGHT };
int mapOffsetY = 0; // Độ dời của map theo trục Y
struct Wall {
    SDL_Rect rect;
    Wall(int x, int y, int w, int h) { rect = { x, y, w, h }; }
    void render() {
        SDL_RenderCopy(renderer, wallTexture, NULL, &rect);
    }
};
std::vector<Wall> walls = {
{100, 100, 40, 40}, {140, 100, 40, 40}, {180, 100, 40, 40}, // 150x40 -> 3 ô
{300, 50, 40, 40}, {300, 90, 40, 40}, {300, 130, 40, 40}, {300, 170, 40, 40}, {300, 210, 40, 40}, // 40x200 -> 5 ô
{500, 200, 40, 40}, {540, 200, 40, 40}, {580, 200, 40, 40}, {620, 200, 40, 40}, {660, 200, 40, 40}, // 200x40 -> 5 ô
{50, 300, 40, 40}, {90, 300, 40, 40}, {130, 300, 40, 40}, // 120x40 -> 3 ô
{250, 400, 40, 40}, {290, 400, 40, 40}, {330, 400, 40, 40}, {370, 400, 40, 40}, // 160x40 -> 4 ô
{450, 100, 40, 40}, {450, 140, 40, 40}, {450, 180, 40, 40}, {450, 220, 40, 40}, // 40x160 -> 4 ô
{600, 300, 40, 40}, {600, 340, 40, 40}, {600, 380, 40, 40}, {600, 420, 40, 40}, {600, 460, 40, 40}, // 40x200 -> 5 ô
{720, 500, 40, 40}, {760, 500, 40, 40}, // 100x40 -> 4 ô
{150, 500, 40, 40}, {190, 500, 40, 40}, {230, 500, 40, 40}, {270, 500, 40, 40}, {310, 500, 40, 40}, // 200x40 -> 5 ô
{350, 250, 40, 40}, {390, 250, 40, 40}, {430, 250, 40, 40}, // 120x40 -> 3 ô
{550, 400, 40, 40}, {590, 400, 40, 40}, {630, 400, 40, 40}, {670, 400, 40, 40}, // 160x40 -> 4 ô
{100, 550, 40, 40}, {100, 590, 40, 40}, {100, 630, 40, 40}, // 40x100 -> 3 ô
{500, 550, 40, 40}, {540, 550, 40, 40}, {580, 550, 40, 40}, {620, 550, 40, 40}, {660, 550, 40, 40}, // 200x40 -> 5 ô
{650, 50, 40, 40}, {690, 50, 40, 40}, {730, 50, 40, 40}, // 120x40 -> 3 ô
{750, 150, 40, 40}, {750, 190, 40, 40}, {750, 230, 40, 40}, // 40x120 -> 3 ô
};
std::vector<std::vector<Wall>> wallPatterns = {
    { {100, 100, 40, 40}, {140, 100, 40, 40}, {180, 100, 40, 40} }, // Mẫu 0
    { {300, 50, 40, 40}, {300, 90, 40, 40}, {300, 130, 40, 40}, {300, 170, 40, 40} }, // Mẫu 1
    { {500, 200, 40, 40}, {540, 200, 40, 40}, {580, 200, 40, 40}, {620, 200, 40, 40} } // Mẫu 2
};

struct Bullet {
    int x, y, speed;
    SDL_Rect rect;
    Direction direction;
    bool active;

    Bullet(int startX, int startY, Direction dir) {
        direction = dir;
        speed = 8;
        active = true;

        //vị trí đạn xuất phát hợp với nòng súng
        switch (direction) {
        case UP:
            x = startX + 15;
            y = startY + 5;  
            break;
        case DOWN:
            x = startX + 15;
            y = startY + 25;
            break;
        case LEFT:
            x = startX + 5; 
            y = startY + 15;
            break;
        case RIGHT:
            x = startX + 25; 
            y = startY + 15;
            break;
        }

        rect = { x, y, 10, 10 };
    }

    void update(std::vector<Wall>& walls) {
        if (!active) return;

        switch (direction) {
        case UP: y -= speed; break;
        case DOWN: y += speed; break;
        case LEFT: x -= speed; break;
        case RIGHT: x += speed; break;
        }

        rect.x = x;
        rect.y = y;

        if (x < 0 || x > 800 || y < 0 || y > SCREEN_HEIGHT) {
            active = false;
            return;
        }

        for (auto& wall : walls) {
            if (SDL_HasIntersection(&rect, &wall.rect)) {
                active = false;
                return;
            }
        }
    }

    void render() {
        SDL_RenderCopy(renderer, bulletTexture, NULL, &rect);
    }
};

struct Tank {
    int x, y, speed, lives = 3;
    double angle;
    SDL_Rect rect;
    Direction direction;
    std::vector<Bullet> bullets;
    Mix_Chunk* fireSound;
    Mix_Chunk* moveSound; // Thêm âm thanh di chuyển
    bool keys[4] = { false, false, false, false }; // Trạng thái phím UP, DOWN, LEFT, RIGHT

    Tank(int startX, int startY) {
        x = startX;
        y = startY;
        speed = 3;
        rect = { x, y, 40, 40 };
        direction = UP;
        angle = 0;

        fireSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\tieng_sung.wav");
        if (!fireSound) {
            printf("Failed to load fire sound! SDL_mixer Error: %s\n", Mix_GetError());
        }

        moveSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\move.wav");
        if (!moveSound) {
            printf("Failed to load move sound! SDL_mixer Error: %s\n", Mix_GetError());
        }
    }

    ~Tank() {
        Mix_FreeChunk(fireSound);
        Mix_FreeChunk(moveSound); // Giải phóng âm thanh di chuyển
        fireSound = nullptr;
        moveSound = nullptr;
    }

    void handleEvent(SDL_Event& e) {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) { // Nhấn phím
            switch (e.key.keysym.sym) {
            case SDLK_UP:
                keys[0] = true; direction = UP; angle = 0;
                Mix_PlayChannel(-1, moveSound, 0); // Phát âm thanh di chuyển
                break;
            case SDLK_DOWN:
                keys[1] = true; direction = DOWN; angle = 180;
                Mix_PlayChannel(-1, moveSound, 0);
                break;
            case SDLK_LEFT:
                keys[2] = true; direction = LEFT; angle = 270;
                Mix_PlayChannel(-1, moveSound, 0);
                break;
            case SDLK_RIGHT:
                keys[3] = true; direction = RIGHT; angle = 90;
                Mix_PlayChannel(-1, moveSound, 0);
                break;
            case SDLK_SPACE:
                bullets.emplace_back(x, y, direction);
                if (fireSound) Mix_PlayChannel(-1, fireSound, 0);
                break;
            }
        }
        else if (e.type == SDL_KEYUP && e.key.repeat == 0) { // Nhả phím
            switch (e.key.keysym.sym) {
            case SDLK_UP:    keys[0] = false; break;
            case SDLK_DOWN:  keys[1] = false; break;
            case SDLK_LEFT:  keys[2] = false; break;
            case SDLK_RIGHT: keys[3] = false; break;
            }
        }
    }

    void reset(int startX, int startY) {
        x = startX;
        y = startY;
        direction = UP;
        angle = 0;
        for (int i = 0; i < 4; i++) keys[i] = 0;
        fireSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\tieng_sung.wav");
        moveSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\move.wav");
    }

    void update(std::vector<Wall>& walls);

    void render() {
        SDL_RenderCopyEx(renderer, tankTexture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
        for (auto& bullet : bullets) bullet.render();
    }
};
Tank playerTank(800 / 2, SCREEN_HEIGHT - 60);
struct EnemyTank {
    int x, y, speed;
    double angle; // Góc quay
    SDL_Rect rect;
    Direction direction;
    bool alive;
    std::vector<Bullet> bullets;
    Uint32 lastShotTime;
    Uint32 lastChangeTime;
    Uint32 changeInterval;
    Mix_Chunk* hitSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\shot.wav");

    EnemyTank(int startX, int startY) {
        x = startX;
        y = startY;
        speed = 2;
        rect = { x, y, 40, 40 };
        direction = static_cast<Direction>(rand() % 4);
        angle = getAngleFromDirection(direction);
        alive = true;
        lastShotTime = SDL_GetTicks();
        lastChangeTime = SDL_GetTicks();
        changeInterval = 1000 + rand() % 2000;
    }

    void update(std::vector<Wall>& walls, Tank& player, std::vector<EnemyTank>& enemies) {
        Uint32 currentTime = SDL_GetTicks();

        // 🚀 Thay đổi hướng đi nếu hết thời gian di chuyển
        if (currentTime - lastChangeTime >= changeInterval) {
            int dx = player.x - x;
            int dy = player.y - y;

            // Ưu tiên di chuyển theo trục có khoảng cách lớn hơn
            if (abs(dx) > abs(dy)) {
                direction = (dx > 0) ? RIGHT : LEFT;
            }
            else {
                direction = (dy > 0) ? DOWN : UP;
            }

            angle = getAngleFromDirection(direction);
            lastChangeTime = currentTime;
            changeInterval = 1000 + rand() % 1500;  // Thời gian thay đổi ngẫu nhiên
        }

        if (alive) {
            int newX = x, newY = y;
            switch (direction) {
            case UP: newY -= speed; break;
            case DOWN: newY += speed; break;
            case LEFT: newX -= speed; break;
            case RIGHT: newX += speed; break;
            }

            SDL_Rect newRect = { newX, newY, 40, 40 };

            // 🚧 Kiểm tra va chạm với biên bản đồ
            if (newX < 0 || newX + 40 > 800 || newY < 0 || newY + 40 > 600) {
                direction = getNewDirection(direction);
                angle = getAngleFromDirection(direction);
            }
            else {
                bool collided = false;

                // 🚧 Kiểm tra va chạm với tường
                for (auto& wall : walls) {
                    if (SDL_HasIntersection(&newRect, &wall.rect)) {
                        direction = getNewDirection(direction);
                        angle = getAngleFromDirection(direction);
                        collided = true;
                        break;
                    }
                }

                // 🏎️ Kiểm tra va chạm với xe tăng khác
                for (auto& enemy : enemies) {
                    if (&enemy != this && SDL_HasIntersection(&newRect, &enemy.rect)) {
                        direction = getNewDirection(direction);
                        angle = getAngleFromDirection(direction);
                        collided = true;
                        break;
                    }
                }
                // 🚗💥 Kiểm tra va chạm với xe chính
                if (SDL_HasIntersection(&newRect, &player.rect)) {
                    direction = getNewDirection(direction);
                    angle = getAngleFromDirection(direction);
                    collided = true;
                }
                // Nếu không va chạm, di chuyển xe
                if (!collided) {
                    x = newX;
                    y = newY;
                    rect.x = x;
                    rect.y = y;
                }
            }

            // 🔫 Địch bắn ngẫu nhiên
            if (rand() % 70 == 0) {
                bullets.emplace_back(x, y, direction);
            }
        }

        // 🔄 Cập nhật và xóa đạn không còn hoạt động
        for (auto& bullet : bullets) bullet.update(walls);
        bullets.erase(remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());

        for (auto& bullet : bullets) {
            if (bullet.active && SDL_HasIntersection(&bullet.rect, &playerTank.rect)) {
                playerTank.lives--;
                bullet.active = false; // Vô hiệu hóa viên đạn sau khi va chạm

                Mix_PlayChannel(-1, hitSound, 0); // Phát âm thanh trúng đạn nếu có

                if (playerTank.lives <= 0) {
                    gameOver = 1; // Kết thúc trò chơi nếu hết mạng
                    return;
                }
            }
        }

        // 🚀 Kiểm tra va chạm với đạn của người chơi
        for (auto& bullet : player.bullets) {
            if (SDL_HasIntersection(&rect, &bullet.rect) && bullet.active) {
                alive = false;
                score++;
                if (score > maxScore) maxScore = score;
                bullet.active = false;
                return;
            }
        }
    }

    // 🎯 Hàm chọn hướng mới, tránh hướng cũ
    Direction getNewDirection(Direction oldDirection) {
        Direction newDir;
        do {
            newDir = static_cast<Direction>(rand() % 4);
        } while (newDir == oldDirection);
        return newDir;
    }

    // 🎯 Hàm chuyển đổi Direction thành góc quay
    double getAngleFromDirection(Direction dir) {
        switch (dir) {
        case UP: return 0;
        case RIGHT: return 90;
        case DOWN: return 180;
        case LEFT: return 270;
        }
        return 0;
    }

    void render() {
        if (alive) {
            SDL_RenderCopyEx(renderer, enemyTankTexture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
        }
        for (auto& bullet : bullets) bullet.render();
    }
};

std::vector<EnemyTank> enemies = {
     {100, 100}, {300, 150}, {50, 100}, {250, 70}, {600, 100}, {200, 450}
};

void Tank:: update(std::vector<Wall>& walls) {
    int newX = x, newY = y;

    // Xử lý di chuyển liên tục
    if (keys[0]) newY -= speed; // UP
    if (keys[1]) newY += speed; // DOWN
    if (keys[2]) newX -= speed; // LEFT
    if (keys[3]) newX += speed; // RIGHT

    // Giữ trong màn hình
    if (newX < 0) newX = 0;
    if (newX > 800 - 40) newX = 800 - 40;
    if (newY < 0) newY = 0;
    if (newY > 600 - 40) newY = 600 - 40;

    // Kiểm tra va chạm với tường
    SDL_Rect newRect = { newX, newY, 40, 40 };
    for (auto& wall : walls) {
        if (SDL_HasIntersection(&newRect, &wall.rect)) return;
    }
    // 🚗💥 Kiểm tra va chạm với xe địch
    for (auto& enemy : enemies) {
        if (SDL_HasIntersection(&newRect, &enemy.rect)) return; // Không di chuyển nếu chạm xe địch
    }
    // Cập nhật vị trí
    x = newX;
    y = newY;
    rect.x = x;
    rect.y = y;

    // Cập nhật đạn
    for (auto& bullet : bullets) bullet.update(walls);
    bullets.erase(remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "Failed to load image: " << path << " SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
// tải nền manu chính
bool loadBackground() {
    SDL_Surface* surface = IMG_Load("C:\\Users\\ACER\\Downloads\\bgrmenu.png");
    if (!surface) {
        std::cerr << "Failed to load background image! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return backgroundTexture != nullptr;
}
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Lỗi khởi tạo SDL: %s\n", SDL_GetError());
        return -1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    window = SDL_CreateWindow("Battle Tanks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}
bool loadGameTextures() {
    tankTexture = loadTexture("C:\\Users\\ACER\\Downloads\\tank1 (2).png");
    enemyTankTexture = loadTexture("C:\\Users\\ACER\\Downloads\\tank2 (2).png");
    wallTexture = loadTexture("C:\\Users\\ACER\\Downloads\\wall_pixel.png");
    bulletTexture = loadTexture("C:\\Users\\ACER\\Downloads\\bullet2.png");
    backgroundTexture2=loadTexture("C:\\Users\\ACER\\Downloads\\nen_nau.jpg");
    background_multitasking = IMG_LoadTexture(renderer,"C:\\Users\\ACER\\Downloads\\nen_da_nhiem.png");
    pauseButtonTexture = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\pause_pixel.png");
    lives1 = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\1live.png");
    lives2 = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\2lives.png");
    lives3 = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\3lives.png");
    font2 = TTF_OpenFont("C:\\VClib\\TCVN3-ABC-fonts\\VHCENTN.TTF", 40);
    font3 = TTF_OpenFont("C:\\Users\\ACER\\Downloads\\font-chu-pixel\\Pixel Sans Serif.ttf", 15);
    if (!font2) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return -1;
    }

    return tankTexture && enemyTankTexture && wallTexture && bulletTexture;
}
//xử lí chữ trong bảng pause
void renderText(const std::string& text, int x, int y) {
    SDL_Color textColor = { 255, 0, 0, 255 }; // Màu chữ (đỏ)
    SDL_Surface* textSurface = TTF_RenderText_Solid(font2, text.c_str(), textColor);
    if (!textSurface) {
        std::cout << "Failed to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture) {
        std::cout << "Failed to create texture from text! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

void showGameOverScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Load font và render chữ "Game Over" (Cần thư viện SDL_ttf)
    TTF_Font* font = TTF_OpenFont("C:\\VClib\\TCVN3-ABC-fonts\\VHCENTN.TTF", 50);
    SDL_Color color = { 255, 0, 0, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME OVER", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50 };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    // Hiển thị menu Restart/Quit
    SDL_Surface* restartSurface = TTF_RenderText_Solid(font, "Press R to Restart", color);
    SDL_Texture* restartTexture = SDL_CreateTextureFromSurface(renderer, restartSurface);
    SDL_Rect restartRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 200, 40 };
    SDL_RenderCopy(renderer, restartTexture, NULL, &restartRect);

    SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "Press Q to Quit", color);
    SDL_Texture* quitTexture = SDL_CreateTextureFromSurface(renderer, quitSurface);
    SDL_Rect quitRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 70, 200, 40 };
    SDL_RenderCopy(renderer, quitTexture, NULL, &quitRect);

    SDL_RenderPresent(renderer);

    // Giải phóng bộ nhớ
    SDL_FreeSurface(surface);
    SDL_FreeSurface(restartSurface);
    SDL_FreeSurface(quitSurface);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(restartTexture);
    SDL_DestroyTexture(quitTexture);
    TTF_CloseFont(font);
}
void showMainMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
    TTF_Font* font = TTF_OpenFont("C:\\Users\\ACER\\Downloads\\lazy foo\\16_true_type_fonts\\lazy.ttf", 50);
    SDL_Color color = { 255, 255, 255, 255 };

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "BATTLE CITY", color);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = { SCREEN_WIDTH / 2 - 150, 100, 300, 50 };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_Surface* startSurface = TTF_RenderText_Solid(font, "START", color);
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_Rect startRect = { SCREEN_WIDTH / 2 - 50, 250, 100, 50 };
    SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

    SDL_Surface* quitSurface = TTF_RenderText_Solid(font, "QUIT", color);
    SDL_Texture* quitTexture = SDL_CreateTextureFromSurface(renderer, quitSurface);
    SDL_Rect quitRect = { SCREEN_WIDTH / 2 - 50, 350, 100, 50 };
    SDL_RenderCopy(renderer, quitTexture, NULL, &quitRect);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(titleSurface);
    SDL_FreeSurface(startSurface);
    SDL_FreeSurface(quitSurface);
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(quitTexture);
    TTF_CloseFont(font);
}
void showPauseMenu() {
    // Bật chế độ hòa trộn để vẽ màu trong suốt
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Màu nền trong suốt (đen, alpha = 128)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_Rect fullScreenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &fullScreenRect);

    // Vẽ các nút menu
    renderText("Continue", 400, 220);
    renderText("Restart", 400, 270);
    renderText("Quit", 400, 320);
}
void reset(Tank& playerTank, std::vector<EnemyTank>& enemies) {
    playerTank.reset(400, 540);
    score = 0;
    playerTank.lives = 3;
    enemies = { {100, 100}, {300, 150}, {50, 100}, {250, 70}, {600, 100}, {200, 450} };
}
void quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();
}
void rendermaxScore() {
    SDL_Color black = { 0, 0, 0, 255 };
    std::string scoreText = "Max Score: " + std::to_string(maxScore);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font3, scoreText.c_str(), black);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect scoreRect = { 815, 400, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &scoreRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}
void renderScore() {
    SDL_Color black = { 0, 0, 0, 255 };
    std::string scoreText = "Score: " + std::to_string(score);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font3, scoreText.c_str(), black);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect scoreRect = { 815, 300, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &scoreRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}
void update_live() {
    switch (playerTank.lives) {
    case 3:
        SDL_RenderCopy(renderer, lives3, NULL, &Live);
        break;
    case 2:
        SDL_RenderCopy(renderer, lives2, NULL, &Live);
        break;
    case 1:
        SDL_RenderCopy(renderer, lives1, NULL, &Live);
        break;
    }
}
int main() {
    srand(time(0));
    if (!initSDL()) return -1;
    int max_score = 0;
    loadBackground();//gọi nền menu chính
    loadGameTextures();
    std::vector<Wall> walls = {
{100, 100, 40, 40}, {140, 100, 40, 40}, {180, 100, 40, 40}, // 150x40 -> 3 ô
{300, 50, 40, 40}, {300, 90, 40, 40}, {300, 130, 40, 40}, {300, 170, 40, 40}, {300, 210, 40, 40}, // 40x200 -> 5 ô
{500, 200, 40, 40}, {540, 200, 40, 40}, {580, 200, 40, 40}, {620, 200, 40, 40}, {660, 200, 40, 40}, // 200x40 -> 5 ô
{50, 300, 40, 40}, {90, 300, 40, 40}, {130, 300, 40, 40}, // 120x40 -> 3 ô
{250, 400, 40, 40}, {290, 400, 40, 40}, {330, 400, 40, 40}, {370, 400, 40, 40}, // 160x40 -> 4 ô
{450, 100, 40, 40}, {450, 140, 40, 40}, {450, 180, 40, 40}, {450, 220, 40, 40}, // 40x160 -> 4 ô
{600, 300, 40, 40}, {600, 340, 40, 40}, {600, 380, 40, 40}, {600, 420, 40, 40}, {600, 460, 40, 40}, // 40x200 -> 5 ô
{720, 500, 40, 40}, {760, 500, 40, 40}, // 100x40 -> 4 ô
{150, 500, 40, 40}, {190, 500, 40, 40}, {230, 500, 40, 40}, {270, 500, 40, 40}, {310, 500, 40, 40}, // 200x40 -> 5 ô
{350, 250, 40, 40}, {390, 250, 40, 40}, {430, 250, 40, 40}, // 120x40 -> 3 ô
{550, 400, 40, 40}, {590, 400, 40, 40}, {630, 400, 40, 40}, {670, 400, 40, 40}, // 160x40 -> 4 ô
{100, 550, 40, 40}, {100, 590, 40, 40}, {100, 630, 40, 40}, // 40x100 -> 3 ô
{500, 550, 40, 40}, {540, 550, 40, 40}, {580, 550, 40, 40}, {620, 550, 40, 40}, {660, 550, 40, 40}, // 200x40 -> 5 ô
{650, 50, 40, 40}, {690, 50, 40, 40}, {730, 50, 40, 40}, // 120x40 -> 3 ô
{750, 150, 40, 40}, {750, 190, 40, 40}, {750, 230, 40, 40}, // 40x120 -> 3 ô
    };

    while (running) {
        SDL_RenderCopy(renderer, backgroundTexture2, NULL, NULL);
        SDL_Event e;
        while (SDL_PollEvent(&e)) {

            auto it = enemies.begin();
            //xóa xe địch đã chết
            while (it != enemies.end()) {
                if (!it->alive) {
                    it = enemies.erase(it); // Xóa enemy đã chết
                }
                else {
                    ++it;
                }
            }
            if (e.type == SDL_QUIT) running = false;
            // xử lí pause
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (!isPaused && x >= pauseButtonRect.x && x <= pauseButtonRect.x + pauseButtonRect.w &&
                    y >= pauseButtonRect.y && y <= pauseButtonRect.y + pauseButtonRect.h) {
                    isPaused = true; // Bật menu tạm dừng
                }
                if (isPaused) {
                    if (x >= 400 && x <= 650) {
                        if (y >= 220 && y <= 250) isPaused = false; // Tiếp tục
                        else if (y >= 270 && y <= 300) {
                            reset(playerTank, enemies);
                            isPaused = false;
                        }
                        else if (y >= 320 && y <= 350) {
                            inMenu = true;
                            isPaused = false;
                        }
                    }
                }
            }      
            if (gameOver) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_r) {  // Restart game
                        gameOver = false;
                        inMenu = false;
                        running = true;
                        reset(playerTank, enemies);
                    }
                    if (e.key.keysym.sym == SDLK_q) {  // Quay lại menu chính
                        gameOver = false;  // ✅ Đảm bảo đặt về false khi quay lại menu
                        inMenu = true;
                        running = true;

                    }
                }
                continue;
            }
            if (inMenu) {
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int x = e.button.x, y = e.button.y;
                    if (x >= SCREEN_WIDTH / 2 - 50 && x <= SCREEN_WIDTH / 2 + 50) {
                        if (y >= 250 && y <= 300) {
                            inMenu = false; // Bắt đầu game
                            gameOver = false;  // ✅ Đảm bảo reset gameOver khi vào game
                            running = true;
                            reset(playerTank, enemies);
                        }
                        else if (y >= 350 && y <= 400) {
                            running = false; // Thoát game
                        }
                    }
                }
            }
            playerTank.handleEvent(e);
        }
        if (inMenu) {
            showMainMenu();
            SDL_Delay(100);
            continue;
        }
        if (gameOver) {
            showGameOverScreen();
            SDL_Delay(100);
            continue;
        }
        if (isPaused) {
            showPauseMenu();
            SDL_RenderPresent(renderer);
            SDL_Delay(100);
            continue;
        }
        playerTank.update(walls);
        for (auto& enemy : enemies) enemy.update(walls, playerTank, enemies);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture2, NULL, NULL);//hiển thị bgr
       
        for (auto& wall : walls) wall.render();
        playerTank.render();
        for (auto& enemy : enemies) enemy.render();
        SDL_RenderCopy(renderer, background_multitasking, NULL, &multitaskingRect);
        SDL_RenderCopy(renderer, background_multitasking, NULL, &multitaskingRect);
        SDL_RenderCopy(renderer, pauseButtonTexture, NULL, &pauseButtonRect);// hiển thị nút pause
        update_live();
        renderScore();
        rendermaxScore();
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    quit();
}
