#pragma once
#include"lib_sdl.h"
#include"init_veriable.h"
#include"tank.h"
#include<vector>
#include"wall2.h"
struct reWall {
    int x, y;
    SDL_Rect rect;
    Uint32 spawnTime;

    reWall(int startX, int startY) {
        x = startX;
        y = startY;
        rect = { x, y, 30, 30 };
        spawnTime = SDL_GetTicks();
    }

    void render() {
        Uint32 elapsed = SDL_GetTicks() - spawnTime;
        Uint8 alpha = 255;

        // Nhấp nháy liên tục trong suốt 5 giây
        if (elapsed <= 5000) {
            Uint32 cycle = elapsed % 500; // Mỗi chu kỳ nhấp nháy kéo dài 500ms
            if (cycle < 250) {
                alpha = (cycle * 255) / 250;    // fade in
            }
            else {
                alpha = ((500 - cycle) * 255) / 250; // fade out
            }
        }

        SDL_SetTextureAlphaMod(reWall_tex, alpha); // Cập nhật alpha cho texture
        SDL_RenderCopy(renderer, reWall_tex, NULL, &rect);
        SDL_SetTextureAlphaMod(reWall_tex, 255); // Reset alpha sau khi vẽ
    }

    bool era() {
        if (SDL_HasIntersection(&rect, &playerTank.rect)) {
            Mix_Chunk* take_item = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\take_item.wav"); 
            Mix_PlayChannel(-1, take_item, 0);
            wall2s.clear();
            init_wall2();
            return 1;
        }
        return SDL_GetTicks() - spawnTime > 5000; // Biến mất sau 5 giây
    }
};

vector<reWall> reW;