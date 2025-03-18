#pragma once
#include"init_veriable.h"
#include"lib_sdl.h"
#include"vector"
#include"wall.h"
#include"explosion.h"
struct Bullet {
    int x, y, speed;
    SDL_Rect rect;
    Direction direction;
    bool active;
    double angle; // Góc xoay
    Bullet() {}  // ✅ Constructor mặc định
    Bullet(int _x, int _y, int _dir) { // Kiểm tra xem có constructor này không
        x = _x;
        y = _y;
        direction = (Direction)_dir;
    }
    Bullet(int startX, int startY, Direction dir) {
        direction = dir;
        speed = 8;
        active = true;

        // Xác định góc xoay và vị trí viên đạn xuất phát chính giữa xe tăng
        switch (direction) {
        case UP:
            angle = 0;
            x = startX + 14;  // Chính giữa xe
            y = startY;        // Cao hơn một chút
            break;
        case DOWN:
            angle = 180;
            x = startX + 15;
            y = startY + 30;
            break;
        case LEFT:
            angle = 270;
            x = startX;        // Lùi sang trái một chút
            y = startY + 15;
            break;
        case RIGHT:
            angle = 90;
            x = startX + 30;   // Tiến sang phải một chút
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

        // Kiểm tra ra ngoài màn hình
        if (x < 0 || x > 800 || y < 0 || y > SCREEN_HEIGHT) {
            active = false;
            return;
        }

        // Kiểm tra va chạm với tường
        for (auto& wall : walls) {
            if (SDL_HasIntersection(&rect, &wall.rect)) {
                active = false;

                // Căn chỉnh vị trí vụ nổ vào chính giữa viên đạn
                int explosionX = rect.x + rect.w / 2 - 15; // 20 là nửa kích thước vụ nổ (40x40)
                int explosionY = rect.y + rect.h / 2 - 15;

                explosions.emplace_back(explosionX, explosionY);
                return;
            }
        }

    }
    void render(SDL_Renderer* renderer, SDL_Texture* bulletTexture) {
        SDL_RenderCopyEx(renderer, bulletTexture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
    }
};


