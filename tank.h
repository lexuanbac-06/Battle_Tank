﻿#pragma once
#include"wall.h"
#include"wall2.h"
#include"lib_sdl.h"
#include"init_veriable.h"
#include"boss.h"
#include<vector>
struct EnemyTank;

struct Tank {
    int x, y, speed, lives = 3;
    double angle;
    SDL_Rect rect;
    Direction direction;
    std::vector<Bullet> bullets;
    int mode2 = 0;
    int spB = 0;
    bool keys[4] = { false, false, false, false }; // Trạng thái phím UP, DOWN, LEFT, RIGHT
    Tank(int startX, int startY) {
        x = startX;
        y = startY;
        speed = 3;
        rect = { x, y, 40, 40 };
        direction = UP;
        angle = 0;
    }

    Uint32 lastShotTime = 0;
    Uint32 shotCooldown=0;

    void shoot() {
        if (spB) shotCooldown = 250;
        else shotCooldown = 500;
        if (SDL_GetTicks() - cooldownStart >= cooldownTime) {
            cooldownStart = SDL_GetTicks();  // Reset hồi chiêu
        }
        Uint32 currentTime = SDL_GetTicks();

        if (currentTime - lastShotTime >= shotCooldown) {
            Mix_Chunk* fireSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\shoot.wav");
            if (spB) {
                spB--;
                if (direction == LEFT || direction == RIGHT) {
                    bullets.emplace_back(x, y - 10, direction); 
                    bullets.emplace_back(x, y-1, direction);     
                    bullets.emplace_back(x, y + 8, direction);  
                }
                else {
                    bullets.emplace_back(x - 10, y, direction);
                    bullets.emplace_back(x-1, y, direction);
                    bullets.emplace_back(x + 8, y, direction);
                }
            }

            else bullets.emplace_back(x, y, direction);
            lastShotTime = currentTime; // Cập nhật thời gian bắn
            Mix_PlayChannel(-1, fireSound, 0);
        }
    }
    
    void handleEvent(SDL_Event& e) {
        if (mode2 == 0) {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) { // Nhấn phím
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    keys[0] = true; direction = UP; angle = 0;
                    break;
                case SDLK_DOWN:
                    keys[1] = true; direction = DOWN; angle = 180;
                    break;
                case SDLK_LEFT:
                    keys[2] = true; direction = LEFT; angle = 270;
                    break;
                case SDLK_RIGHT:
                    keys[3] = true; direction = RIGHT; angle = 90;
                    break;
                case SDLK_SPACE:
                    shoot();
                    break;
                case SDLK_RSHIFT:
                    shoot();
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
        else {
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) { // Nhấn phím
                switch (e.key.keysym.sym) {
                case SDLK_w:
                    keys[0] = true; direction = UP; angle = 0;
                    break;
                case SDLK_s:
                    keys[1] = true; direction = DOWN; angle = 180;
                    break;
                case SDLK_a:
                    keys[2] = true; direction = LEFT; angle = 270;
                    break;
                case SDLK_d:
                    keys[3] = true; direction = RIGHT; angle = 90;
                    break;
                case SDLK_LSHIFT:
                    shoot();
                    break;
                }
            }
            else if (e.type == SDL_KEYUP && e.key.repeat == 0) { // Nhả phím
                switch (e.key.keysym.sym) {
                case SDLK_w:    keys[0] = false; break;
                case SDLK_s:  keys[1] = false; break;
                case SDLK_a:  keys[2] = false; break;
                case SDLK_d: keys[3] = false; break;
                }
            }

        }
    }

    

    void update(vector<Wall>& walls, vector<Wall2>& wall2s, Boss& boss,vector <EnemyTank>& enemies);
    Uint32 hitTimer = 0;  // Thời gian còn lại để nhấp nháy
    const Uint32 hitDuration = 2000; // Nhấp nháy trong 2 giây

    void takeDamage() {
        hitTimer = SDL_GetTicks() + hitDuration; // Bắt đầu hiệu ứng nhấp nháy
    }

    void render() {
        Uint32 currentTime = SDL_GetTicks();

        if (hitTimer > currentTime && mode_2==0) {
            // Chỉ hiển thị tank mỗi 100ms để tạo hiệu ứng nhấp nháy
            if ((currentTime / 100) % 2 == 0) return;
        }

        if (mode2 == 0) SDL_RenderCopyEx(renderer, tankTexture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
        if (mode2 == 1) SDL_RenderCopyEx(renderer, enemyTankTexture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);

        for (auto& bullet : bullets) bullet.render(renderer, bulletTexture);
    }
    void reset(int startX, int startY) {
        x = startX;
        y = startY;
        spB = 0;
        bullets.clear();
        direction = UP;
        angle = 0;
        lives = 3;
        for (int i = 0; i < 4; i++) keys[i] = 0;
        hitTimer = 0;
    }
};
Tank playerTank(800 / 2, SCREEN_HEIGHT - 320);

Tank player1(200, 400);

Tank player2(560, 400);
