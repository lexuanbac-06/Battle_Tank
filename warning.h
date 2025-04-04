#pragma once
#include"lib_sdl.h"
#include"init_veriable.h"
bool warningActive = false;
Uint32 warningStartTime = 0;
void startWarning() {
    warningActive = true;
    warningStartTime = SDL_GetTicks(); // lưu thời gian bắt đầu 
}

void renderWarning() {
    if (!warningActive) return; // Không làm gì n?u ch?a kích ho?t

    Uint32 elapsed = SDL_GetTicks() - warningStartTime; // Tính thoi gian dã trôi qua
    if (elapsed > 500) {
        warningActive = false; // T?t hi?u ?ng sau 0.5s
        return;
    }

    // Tính alpha (?? trong su?t): ??m d?n 250ms ??u, nh?t d?n 250ms sau
    Uint8 alpha = (elapsed < 250) ? (elapsed * 100 / 250) : ((500 - elapsed) * 100 / 250);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Cho phép màu trong su?t
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha); // ??t màu ?? v?i alpha
    SDL_Rect fullScreen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; // V? toàn màn hình
    SDL_RenderFillRect(renderer, &fullScreen);
}
