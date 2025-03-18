#pragma once
#include"lib_sdl.h"
#include"init_veriable.h"
#include"vector"
struct Wall {
    SDL_Rect rect;
    Wall(int x, int y) { rect = { x, y, 40, 40 }; }
    void render() {
        SDL_RenderCopy(renderer, wallTexture, NULL, &rect);
    }
};

vector<Wall> walls = {
    {0,160}, {40,160},
    {120,0}, {120,40},
    {120, 160}, {120,240}, {120,200},
    {240,240},{200,240}, {40,360},{40,400}, {240,200},{240,160},{240,120},{240,80},{240,40},
    {40,240}, {80,320},{160,320},{160,360},{160,400},{240,320},
    {240,360},{240,400},{320,120},{320,160},{320,160},{320,240},{360,160},
    {360,240},{400,160},{400,240},
    {360,40},{400,40}

};

void mirrorWalls() {
    vector<Wall> mirroredWalls = walls; // Sao chép danh sách gốc

    for (const Wall& wall : walls) {
        int x = wall.rect.x;
        int y = wall.rect.y;

        // Đối xứng qua trục dọc
        mirroredWalls.emplace_back(800 - x - 40, y);

        // Đối xứng qua trục ngang
        mirroredWalls.emplace_back(x, 800 - y - 40);

        // Đối xứng qua gốc (400,400)
        mirroredWalls.emplace_back(800 - x - 40, 800 - y - 40);
    }

    walls = mirroredWalls; // Cập nhật lại danh sách tường
}
