#pragma once
#include"lib_sdl.h"
#include"init_veriable.h"
#include"tank.h"
#include<vector>
struct Heart {
    int x, y;
    SDL_Rect rect;
    Uint32 spawnTime;

    Heart(int startX, int startY) {
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

        SDL_SetTextureAlphaMod(heart_tex, alpha); // Cập nhật alpha cho texture
        SDL_RenderCopy(renderer, heart_tex, NULL, &rect);
        SDL_SetTextureAlphaMod(heart_tex, 255); // Reset alpha sau khi vẽ
    }

    bool era() {
        if (mode_2 == 0) {
            if (SDL_HasIntersection(&rect, &playerTank.rect)) {
                Mix_Chunk* take_item = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\take_item.wav");
                Mix_PlayChannel(-1, take_item, 0);
                if (playerTank.lives < 3) playerTank.lives += 1;
                return 1;
            }
        }
        else if(mode_2==1) {
            if (SDL_HasIntersection(&rect, &player1.rect)||SDL_HasIntersection(&rect, &player2.rect)) {
                Mix_Chunk* take_item = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\take_item.wav");
                Mix_PlayChannel(-1, take_item, 0);
                if (SDL_HasIntersection(&rect, &player1.rect)) {
                    if (player1.lives < 3) player1.lives += 1;
                }
                else if (SDL_HasIntersection(&rect, &player2.rect)) {
                    if (player2.lives < 3) player2.lives += 1;
                }
                return 1;
            }
        }
        return SDL_GetTicks() - spawnTime > 5000; // Biến mất sau 5 giây
    }
};

vector<Heart> hearts;