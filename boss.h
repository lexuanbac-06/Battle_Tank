#pragma once
#include"lib_sdl.h"
#include"vector"
#include"init_veriable.h"
#include"bullet.h"
struct Boss {
    int x, y;
    SDL_Rect rect;
    bool alive = true;

    Boss(int _x, int _y) : x(_x), y(_y) {
        rect = { x, y, 40, 40 }; // Boss có kích thước 40x40
    }

    // Kiểm tra va chạm với đạn
    void checkBulletCollision(std::vector<Bullet>& bullets) {
        if (mode_2) return;
        for (auto& bullet : bullets) {
            if (bullet.active && SDL_HasIntersection(&rect, &bullet.rect)) { // Sửa lỗi boss.rect
                alive = false; // Boss chết
                bullet.active = false; // Xóa đạn
                gameOver = 1; // Game over
            }
        }
    }

    // Render boss
    void render() {
        if (mode_2) return;
        if (alive) {
            SDL_RenderCopy(renderer, bossTexture, NULL, &rect);
        }
    }
};
Boss boss(380, 380);
