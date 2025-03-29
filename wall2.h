#pragma once
#include"init_veriable.h"
#include"lib_sdl.h"
#include"vector"
#include"bullet.h"
#include"warning.h"
struct Wall2 {
    int x, y;
    SDL_Rect rect;
    bool destroyed;
    Mix_Chunk* warning = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\warning.wav");      // Âm thanh nổ

    Wall2(int startX, int startY) {
        x = startX;
        y = startY;
        rect = { x, y, 20, 20 };
        destroyed = false;
    }

    void render() {
        if (!destroyed) {
            SDL_RenderCopy(renderer, wall2Texture, NULL, &rect);
        }
    }
    void checkBulletCollision(std::vector<Bullet>& bullets, std::vector<Wall2>& breakableWalls) {
        for (auto& bullet : bullets) {
            if (!bullet.active) continue;

            // Duyệt từng tường trong danh sách
            for (auto it = breakableWalls.begin(); it != breakableWalls.end(); ++it) {
                if (SDL_HasIntersection(&it->rect, &bullet.rect)) {
                    if (it->rect.x > 280 && it->rect.x < 520 &&it->rect.y>280&&it->rect.y<480&& !mode_2) {
                        startWarning();
                        Mix_PlayChannel(-1, warning, 0);
                    }
                    int explosionX = bullet.rect.x + bullet.rect.w / 2 - 15; // 20 là nửa kích thước vụ nổ (40x40)
                    int explosionY = bullet.rect.y + bullet.rect.h / 2 - 15;
                    explosions.emplace_back(explosionX, explosionY);
                    bullet.active = false; // Đạn biến mất
                    // Xóa tường khỏi danh sách
                    breakableWalls.erase(it);
                    return; // Dừng vòng lặp vì iterator bị thay đổi
                }
            }
        }
    }
};

vector<Wall2> wall2s;

void init_wall2() {
    int x = 380, y = 380;
    int size = 20; // Kích thước mỗi ô tường

    // Lớp 1 (sát ô 40x40)
    for (int i = -1; i <= 2; i++) {
        wall2s.emplace_back(x + i * size, y - size); // Hàng trên
        wall2s.emplace_back(x + i * size, y + size * 2); // Hàng dưới
        wall2s.emplace_back(x - size, y + i * size); // Cột trái
        wall2s.emplace_back(x + size * 2, y + i * size); // Cột phải
    }

    // Lớp 2 (ngoài lớp 1)
    for (int i = -2; i <= 3; i++) {
        wall2s.emplace_back(x + i * size, y - size * 2); // Hàng trên
        wall2s.emplace_back(x + i * size, y + size * 3); // Hàng dưới
        wall2s.emplace_back(x - size * 2, y + i * size); // Cột trái
        wall2s.emplace_back(x + size * 3, y + i * size); // Cột phải
    }
    vector<pair<int, int>> positions = {
        {200, 320}, {200, 440}, {560, 320}, {560, 440},
        {320, 200}, {440, 200}, {320, 560}, {440, 560}
    };

    size = 20; // Kích thước của wall2

    for (size_t i = 0; i < positions.size(); i++) {
        int x = positions[i].first;
        int y = positions[i].second;

        wall2s.emplace_back(x, y);
        wall2s.emplace_back(x + size, y);
        wall2s.emplace_back(x, y + size);
        wall2s.emplace_back(x + size, y + size);
    }
}