#pragma once
#include"lib_sdl.h"
#include"init_veriable.h"
struct Grass {
    int x, y;
    SDL_Rect rect;
    Grass(int startX, int startY) {
        x = startX;
        y = startY;
        rect = { x, y, 40, 40 };
    }
    void render() {
         SDL_RenderCopy(renderer, grassTex, NULL, &rect);
    }
};