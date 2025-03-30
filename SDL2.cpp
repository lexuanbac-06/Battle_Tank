#include"lib_sdl.h"
#include<string>
#include<fstream>
#include<iostream>
#include <vector>
#include"init_veriable.h"
#include"wall.h"
#include"wall2.h"
#include"bullet.h"
#include"explosion.h"
#include"warning.h"
#include"boss.h"
#include"enemy.h"
#include"tank.h"
#include"grass.h"
#undef main  
vector<Grass> grass = { {320,680}, {320,720},{440,680}, {440,720}, {120,80}, {120,120},  {640,80}, {640,120}, {200,480}, {560,480} , {160,280}, {600,280},
{ 40,320 }, {40,440},{ 720,320 }, {720,440}, {380,290}, {360,560},{400,560 }};

vector<EnemyTank> enemies = {
     {100, 100}, {300, 150}, {50, 100}, {250, 70}, {600, 100}, {200, 450}
};

//sinh enemy

Uint32 lastSpawnTime = 0;

const Uint32 spawnInterval = 3000; // 3 giây

void spawnEnemyTank() {
    Uint32 currentTime = SDL_GetTicks();

    // 🔴 Chỉ spawn nếu đã đủ thời gian từ lần spawn trước
    if (currentTime - lastSpawnTime < spawnInterval) return;
    int spawnX = rand() % 760;
    int spawnY = rand() % 760;

    SDL_Rect spawnRect = { spawnX, spawnY, 40, 40 };
    bool validSpawn = true;

    for (auto& wall : walls) {
        if (SDL_HasIntersection(&spawnRect, &wall.rect)) {
            validSpawn = false;
            break;
        }
    }
    for (auto& wall : wall2s) {
        if (SDL_HasIntersection(&spawnRect, &wall.rect)) {
            validSpawn = false;
            break;
        }
    }
    for (auto& enemy : enemies) {
        if (SDL_HasIntersection(&spawnRect, &enemy.rect)) {
            validSpawn = false;
            break;
        }
    }
    if (SDL_HasIntersection(&spawnRect, &boss.rect) ||
        SDL_HasIntersection(&spawnRect, &playerTank.rect)) {
        validSpawn = false;
    }

    if (validSpawn) {
        enemies.emplace_back(spawnX, spawnY);
        lastSpawnTime = currentTime; // 🔴 Cập nhật thời gian spawn mới
    }
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
    SDL_Surface* surface = IMG_Load("C:\\Users\\ACER\\Downloads\\nen1.png");
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
// tải ảnh game 
bool loadGameTextures() {
    tankTexture = loadTexture("C:\\Users\\ACER\\Downloads\\tank1 (2) (2).png");
    enemyTankTexture = loadTexture("C:\\Users\\ACER\\Downloads\\tank2 (2).png");
    wallTexture = loadTexture("C:\\Users\\ACER\\Downloads\\wall_pixel.png");
    wall2Texture = loadTexture("C:\\Users\\ACER\\Downloads\\wall2_pixel.png");
    bulletTexture = loadTexture("C:\\Users\\ACER\\Downloads\\bullet_pixel.png");
    bulletTexture2 = loadTexture("C:\\Users\\ACER\\Downloads\\bullet_pixel2.png");
    backgroundTexture2 = loadTexture("C:\\Users\\ACER\\Downloads\\nen_nau.jpg");
    background_multitasking = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\nen_da_nhiem.png");
    pauseButtonTexture = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\pause_pixel.png");
    bossTexture = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\mu_pixel.png");
    grassTex= loadTexture("C:\\Users\\ACER\\Downloads\\co.png");
    lives1 = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\1live.png");
    lives2 = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\2lives.png");
    lives3 = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\3lives.png");
    hs_tex = IMG_LoadTexture(renderer, "C:\\Users\\ACER\\Downloads\\hoi_sinh.png");
    font = TTF_OpenFont("C:\\Users\\ACER\\Downloads\\font-chu-pixel\\Pixel Sans Serif.ttf", 15);
    font2 = TTF_OpenFont("C:\\Users\\ACER\\Downloads\\font-chu-pixel\\Pixel Sans Serif.ttf", 40);
    explosionTextures[0] = loadTexture("C:\\Users\\ACER\\Downloads\\boom1.png");
    explosionTextures[1] = loadTexture("C:\\Users\\ACER\\Downloads\\boom2.png");
    explosionTextures[2] = loadTexture("C:\\Users\\ACER\\Downloads\\boom3.png");
    explosionTextures[3] = loadTexture("C:\\Users\\ACER\\Downloads\\boom4.png");
    return tankTexture && enemyTankTexture && wallTexture && bulletTexture;
}

//xử lí chữ trong bảng pause
bool isMouseOverText = false; // Biến kiểm tra chuột có ở trên chữ không

void renderText(const std::string& text, int x, int y) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Tạo text tạm để lấy kích thước chữ
    SDL_Surface* tempSurface = TTF_RenderText_Solid(font2, text.c_str(), { 0, 0, 0, 255 });
    if (!tempSurface) return;

    SDL_Rect textRect = { x, y, tempSurface->w, tempSurface->h };
    SDL_FreeSurface(tempSurface);

    // Kiểm tra chuột có nằm trên chữ không (chỉ khi ở Main Menu hoặc Pause Menu)
    bool isMouseOverText = (mouseX >= textRect.x && mouseX <= textRect.x + textRect.w &&
        mouseY >= textRect.y && mouseY <= textRect.y + textRect.h) &&
        (isPaused || inMenu);

    // Chọn màu chữ
    SDL_Color textColor = isMouseOverText
        ? SDL_Color{ 255, 255, 0, 255 }  // Vàng sáng khi hover
    : SDL_Color{ 255, 255, 255, 255 };  // Vàng bình thường

    SDL_Color outlineColor = { 0, 0, 0, 255 }; // Màu viền đen
    int outlineOffset = 2; // Độ dày viền

    // Vẽ viền chữ trước
    int offsets[8][2] = {
        {-outlineOffset, -outlineOffset}, {0, -outlineOffset}, {outlineOffset, -outlineOffset},
        {-outlineOffset, 0}, {outlineOffset, 0},
        {-outlineOffset, outlineOffset}, {0, outlineOffset}, {outlineOffset, outlineOffset}
    };

    for (int i = 0; i < 8; i++) {
        SDL_Surface* outlineSurface = TTF_RenderText_Solid(font2, text.c_str(), outlineColor);
        SDL_Texture* outlineTexture = SDL_CreateTextureFromSurface(renderer, outlineSurface);
        SDL_Rect outlineRect = { x + offsets[i][0], y + offsets[i][1], outlineSurface->w, outlineSurface->h };
        SDL_RenderCopy(renderer, outlineTexture, NULL, &outlineRect);
        SDL_FreeSurface(outlineSurface);
        SDL_DestroyTexture(outlineTexture);
    }

    // Vẽ chữ chính
    SDL_Surface* textSurface = TTF_RenderText_Solid(font2, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };

    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void showGameOverScreen() {
    // Bật chế độ hòa trộn để vẽ màu trong suốt
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Màu nền trong suốt (đen, alpha = 128)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_Rect fullScreenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &fullScreenRect);
    if (mode_2 == 0) {
        renderText("G A M E  O V E R", 250, 200);
        string ys;
        if (score > score_tmp) ys = "New High Score!!!: " + to_string(score);
        else ys = "Your Score: " + to_string(score);
        renderText(ys.c_str(), 250, 300);
    }
    else if (player2.lives == 0) renderText("Player1 Win!",300,200);
    else if (player1.lives == 0) renderText("Player2 Win!",300,200);
    renderText("Press r to Restart", 200, 500);
    renderText("Press q to Quit", 250, 600);
    SDL_RenderPresent(renderer);
}

void showMainMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }

    // Vẽ lớp phủ màu đen trong suốt
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_Rect overlayRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlayRect);

    renderText("2 Players", 380, 520);
    renderText("1 Player", 400, 590);
    renderText("Resume", 430, 650);
    renderText("Quit", 460, 710);
    SDL_RenderPresent(renderer);
}//52 59 65 71

void showPauseMenu() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Đặt lại chế độ hòa trộn
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_Rect fullScreenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &fullScreenRect);

    renderText("Continue", 300, 50);
    renderText("Restart", 310, 150);
    if (mode_2 == 0)
    {
        renderText("Save and Quit", 230, 250);
        renderText("Quit", 370, 350);
    }
    else renderText("Quit", 370, 250);
    
}

void renderCooldownIndicator(int x, int y) {
    if (!bulletTexture2) return; // Đảm bảo texture đã được tải trước đó

    Uint32 elapsed = SDL_GetTicks() - cooldownStart;
    if (elapsed >= cooldownTime) {
        // Nếu hồi chiêu xong, hiển thị đầy đủ viên đạn
        SDL_Rect dstRect = { x, y, 100, 100 };
        SDL_RenderCopy(renderer, bulletTexture2, NULL, &dstRect);
        return;
    }

    // Nếu đang hồi chiêu, tính toán phần ảnh hiển thị dần
    float progress = (float)elapsed / cooldownTime; // Tiến trình (0 → 1)

    int fullHeight = 100; // Chiều cao đầy đủ của viên đạn
    int visibleHeight = (int)(progress * fullHeight); // Tính phần ảnh hiển thị

    if (visibleHeight > 0) {  // Chỉ vẽ nếu có phần nào hiển thị
        SDL_Rect srcRect = { 0, fullHeight - visibleHeight, 100, visibleHeight };
        SDL_Rect dstRect = { x, y + (fullHeight - visibleHeight), 100, visibleHeight };
        SDL_RenderCopy(renderer, bulletTexture2, &srcRect, &dstRect);
    }
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
    std::string scoreText = "High Score: " + std::to_string(maxScore);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), black);
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

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), black);
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

void update_live_p1() {
    switch (player1.lives) {
    case 3:
        SDL_RenderCopy(renderer, lives3, NULL, &Live1);
        SDL_RenderCopy(renderer, tankTexture, NULL, &rect_p1);
        break;
    case 2:
        SDL_RenderCopy(renderer, lives2, NULL, &Live1);
        SDL_RenderCopy(renderer, tankTexture, NULL, &rect_p1);

        break;
    case 1:
        SDL_RenderCopy(renderer, lives1, NULL, &Live1);
        SDL_RenderCopy(renderer, tankTexture, NULL, &rect_p1);
        break;
    }
}

void update_live_p2() {
    switch (player2.lives) {
    case 3:
        SDL_RenderCopy(renderer, lives3, NULL, &Live2);
        SDL_RenderCopy(renderer, enemyTankTexture, NULL, &rect_p2);
        break;
    case 2:
        SDL_RenderCopy(renderer, lives2, NULL, &Live2);
        SDL_RenderCopy(renderer, enemyTankTexture, NULL, &rect_p2);

        break;
    case 1:
        SDL_RenderCopy(renderer, lives1, NULL, &Live2);
        SDL_RenderCopy(renderer, enemyTankTexture, NULL, &rect_p2);
        break;
    }
}

void init_maxScore() {
    ifstream in("maxScore.txt");
    if (in) {
        in >> maxScore; // Đọc điểm cao từ file nếu có
    }
    else {
        maxScore = 0; // Nếu file không tồn tại, đặt maxScore = 0
    }
    score_tmp = maxScore;
    in.close();
}

void save_maxScore() {
    ofstream out("maxScore.txt");
    if (out) {
        out << maxScore; // Ghi điểm cao vào file
    }
    out.close();
}

void save_data() {
    ofstream out("data.txt");
    if (!out) {
        cout << "Không thể mở file data.txt để ghi!" << endl;
        return;
    }

    for (auto& wall2 : wall2s) {
        out << wall2.x << " " << wall2.y << endl;
    }
    out << "-1 -1" << endl;
    out << score << " " << playerTank.lives<<endl;
    out << playerTank.x << " " << playerTank.y << endl;

    for (auto& bullet : playerTank.bullets) {
        out << bullet.x << " " << bullet.y << " " << bullet.direction << endl;
    }
    out << "-1 -1 -1" << endl;

    for (auto& enemy : enemies) {
        out << enemy.x << " " << enemy.y << endl;
    }
    out << "-1 -1" << endl;

    for (auto& enemy : enemies) {
        for (auto& bullet : enemy.bullets) {
            out << bullet.x << " " << bullet.y << " " << bullet.direction << endl;
        }
        out << "-1 -1 -1" << endl;
    }

    // 🔴 Lưu thời gian spawn gần nhất
    out << lastSpawnTime << endl;

    out.close();
}

void load_data() {
    ifstream in("data.txt");
    if (!in) {
        cout << "Không thể mở file data.txt!" << endl;
        return;
    }

    wall2s.clear();
    enemies.clear();
    playerTank.bullets.clear();
    for (auto& enemy : enemies) {
        enemy.bullets.clear();
    }
    int x, y, dir;

    while (in >> x >> y) {
        if (x == -1 && y == -1) break;
        wall2s.emplace_back(x, y);
    }
    in >> score >> playerTank.lives;
    in >> playerTank.x >> playerTank.y;
    
    while (in >> x >> y>>dir) {
        if (x == -1 && y == -1&&dir==-1) break;
        playerTank.bullets.emplace_back(x, y, dir);
    }

    while (in >> x >> y) {
        if (x == -1 && y == -1) break;
        enemies.emplace_back(x, y);
    }

    for (auto& enemy : enemies) {
        while (in >> x >> y >> dir) {
            if (x == -1 && y == -1 && dir == -1) break;
            enemy.bullets.emplace_back(x, y, dir);
        }
    }

    // 🔴 Load lại thời gian spawn
    in >> lastSpawnTime;

    in.close();
}

void reset(Tank& playerTank, std::vector<EnemyTank>& enemies, bool keepData = false) {
    if(keepData) load_data();
    else { // Nếu không giữ data (game mới), thì reset hết
        playerTank.reset(800 / 2, SCREEN_HEIGHT - 320);
        score = 0;
        enemies = { {100, 100}, {50, 100}, {600, 100} };
        boss.alive = 1;
        wall2s.clear();
        explosions.clear();
        init_wall2();
        save_data();
    }
}

void check_shoot_mode2() {
    //kiem tra 2 đạn va chạm
    Mix_Chunk* explosionSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\boom.wav");      // Âm thanh nổ
    for (auto& bullet1 : player1.bullets) {
        if (!bullet1.active) continue;

        for (auto& bullet2 : player2.bullets) {
            if (!bullet2.active) continue;

            if (SDL_HasIntersection(&bullet2.rect, &bullet1.rect)) {
                bullet1.active = false;
                bullet2.active = false;

                // Căn chỉnh vị trí vụ nổ vào chính giữa viên đạn
                int explosionX = (bullet1.rect.x + bullet1.rect.w / 2 +
                    bullet2.rect.x + bullet2.rect.w / 2) / 2 - 20; // 20 là nửa kích thước vụ nổ
                int explosionY = (bullet1.rect.y + bullet1.rect.h / 2 +
                    bullet2.rect.y + bullet2.rect.h / 2) / 2 - 20;

                explosions.emplace_back(explosionX, explosionY);
            }
        }
    }
    //kiem tra dan2 va player1
    for (auto& bullet : player2.bullets) {
        if (bullet.active && SDL_HasIntersection(&bullet.rect, &player1.rect)) {
            player1.lives--;
            bullet.active = false;

            // Phát âm thanh ngay khi vụ nổ bắt đầu
            explosions.emplace_back(player1.x+5, player1.y+5); // Thêm hiệu ứng nổ
            if (explosionSound) {
                Mix_PlayChannel(-1, explosionSound, 0);
            }
            if (player1.lives <= 0) {
                gameOver = 1;
                return;
            }
        }
    }
    //kiem tra dan1 va player2
    for (auto& bullet : player1.bullets) {
        if (bullet.active && SDL_HasIntersection(&bullet.rect, &player2.rect)) {
            player2.lives--;
            bullet.active = false;

            if (explosionSound) {
                Mix_PlayChannel(-1, explosionSound, 0);
            }
            explosions.emplace_back(player2.x+5, player2.y+5); // Thêm hiệu ứng nổ

            if (player2.lives <= 0) {
                gameOver = 1;
                return;
            }
        }
    }
}

int main() {
    player2.mode2 = 1;
    init_maxScore();
    mirrorWalls();
    srand(time(0));
    if (!initSDL()) return -1;
    int max_score = 0;
    loadBackground();//gọi nền menu chính
    loadGameTextures();
    init_wall2();
    while (running) {
        SDL_RenderCopy(renderer, backgroundTexture2, NULL, NULL);
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (mode_2 == 0) {
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
                    if (x >= 300 && x <= 600) {
                        if (y >= 50 && y < 150) {
                            isPaused = false; //tiep tuc
                        }
                        else if (y >= 150 && y < 250) {//choi lai
                            if (mode_2 == 0) {
                                reset(playerTank, enemies);
                                isPaused = false;
                            }
                            else {
                                player1.reset(200,400);
                                player2.reset(560, 400);
                                isPaused = false;
                                wall2s.clear();
                                init_wall2();
                                explosions.clear();
                            }
                        }

                        else if (y >= 250 && y < 350) {//luu va thoat
                            if (mode_2 == 0) {
                                inMenu = true;
                                isPaused = false;
                                save_data();
                            }
                            else {
                                inMenu = true;
                                isPaused = false;
                            }
                        }
                        else if (y >= 350 && y <= 450) {//thoat
                            if(mode_2==0){
                                inMenu = true;
                                isPaused = false;
                                reset(playerTank, enemies);
                            }
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
                        if(mode_2==0) reset(playerTank, enemies);
                        if (mode_2 == 1) {
                            player1.reset(200, 400);
                            player2.reset(560, 400);
                            wall2s.clear();
                            init_wall2();
                            explosions.clear();
                        }
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
                if (e.type == SDL_MOUSEBUTTONDOWN) {//52 59 65 71
                    int x = e.button.x, y = e.button.y;
                    if (x >= 400 && x <= 700) {
                        if (y >= 520 && y < 590) {
                            mode_2 = 1;
                            inMenu = false; // Bắt đầu game
                            gameOver = false; 
                            running = true;
                            player1.reset(200, 400);
                            player2.reset(560, 400);
                            wall2s.clear();
                            init_wall2();
                            explosions.clear();
                            enemies.clear();
                        }
                        else if (y >= 590 && y < 650) {
                            inMenu = false; // Bắt đầu game
                            gameOver = false;  // ✅ Đảm bảo reset gameOver khi vào game
                            running = true;
                            mode_2 = 0;
                            reset(playerTank, enemies);
                        }
                        else if (y >= 650 && y < 710) {
                            inMenu = false; // Bắt đầu game
                            gameOver = false;  // ✅ Đảm bảo reset gameOver khi vào game
                            running = true;
                            mode_2 = 0;
                            reset(playerTank, enemies,1);
                        }
                        else if (y >= 710 && y <= 770) {
                            running = false; // Thoát game
                        }
                    }
                }
            }
            if (mode_2==0) playerTank.handleEvent(e);
            if (mode_2 == 1) {
                player1.handleEvent(e);
                player2.handleEvent(e);
            }
        }
        if (inMenu) {
            showMainMenu();
            SDL_Delay(100);
            continue;
        }
        if (gameOver) {
            playerTank.reset(800 / 2, SCREEN_HEIGHT - 320);
            enemies = { {100, 100}, {50, 100}, {600, 100} };
            boss.alive = 1;
            wall2s.clear();
            explosions.clear();
            init_wall2();
            save_data();
            if (!played_soundGO) {
                Mix_Chunk* gameoverSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\gameover.wav"); 
                Mix_PlayChannel(-1, gameoverSound, 0);
                played_soundGO = 1;
            }
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
        if (mode_2 == 0) {
            spawnEnemyTank();
            playerTank.update(walls,wall2s,boss,enemies);
            for (auto& enemy : enemies) enemy.update(walls, playerTank, enemies);
        }
        else {
            player1.update(walls, wall2s, boss, enemies);
            player2.update(walls, wall2s, boss, enemies);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture2, NULL, NULL);//hiển thị bgr
        for (auto& wall : walls) wall.render();
        for (auto& wall : wall2s) wall.render();
        if (mode_2 == 0) {
            if (mode_2 == 0) SDL_RenderCopy(renderer, hs_tex, NULL, &hs);// hiển thị điểm hồi sinh 
            playerTank.render();
            for (auto& enemy : enemies) enemy.render();
        }
        else {
            player1.render();
            player2.render();
        }
        SDL_RenderCopy(renderer, background_multitasking, NULL, &multitaskingRect);
        SDL_RenderCopy(renderer, pauseButtonTexture, NULL, &pauseButtonRect);// hiển thị nút paus
        if(mode_2==0) boss.render();
        if(mode_2==0) renderCooldownIndicator(850, 500); // Hiển thị viên đạn hồi chiêu ở vị trí HUD
      
        for (auto& explosion : explosions) {
            explosion.update();
        }
        for (auto& explosion : explosions) {
            explosion.render(renderer, explosionTextures);
        }
        if (mode_2 == 0) {
            update_live();
            renderScore();
            rendermaxScore();
            renderWarning();
        }
        else {
            update_live_p1();
            update_live_p2();
            check_shoot_mode2();
        }
        for (auto g : grass) g.render();
        played_soundGO = 0;
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    save_maxScore();
    quit();
}
