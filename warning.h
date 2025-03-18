#pragma once
#include"lib_sdl.h"
#include"init_veriable.h"
bool warningActive = false;

void startWarning() {
    warningActive = true;
    warningStartTime = SDL_GetTicks(); // L?u th?i ?i?m b?t ??u
}

void renderWarning() {
    if (!warningActive) return; // Kh�ng l�m g� n?u ch?a k�ch ho?t

    Uint32 elapsed = SDL_GetTicks() - warningStartTime; // T�nh th?i gian ?� tr�i qua
    if (elapsed > 500) {
        warningActive = false; // T?t hi?u ?ng sau 0.5s
        return;
    }

    // T�nh alpha (?? trong su?t): ??m d?n 250ms ??u, nh?t d?n 250ms sau
    Uint8 alpha = (elapsed < 250) ? (elapsed * 100 / 250) : ((500 - elapsed) * 100 / 250);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Cho ph�p m�u trong su?t
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha); // ??t m�u ?? v?i alpha
    SDL_Rect fullScreen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; // V? to�n m�n h�nh
    SDL_RenderFillRect(renderer, &fullScreen);
}
