#pragma once
#include"lib_sdl.h"
#include<vector>
struct Explosion {
    int x, y;
    int frame;
    bool active;
    Uint32 lastFrameTime;
    static constexpr int frameDelay = 100; // 100ms m?i frame
    static constexpr int totalFrames = 5;  // T?ng s? frame hi?u ?ng n?


    Explosion(int _x, int _y) : x(_x), y(_y), frame(0), active(true) {
        lastFrameTime = SDL_GetTicks();
    }

    void update() {
        if (!active) return;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastFrameTime >= frameDelay) {
            lastFrameTime = currentTime;
            frame++;
            if (frame >= totalFrames) {
                active = false; // K?t thúc hi?u ?ng n?
            }
        }
    }

    void render(SDL_Renderer* renderer, SDL_Texture* explosionTextures[]) {
        if (active && frame < totalFrames) {
            SDL_Rect dstRect = { x, y, 30, 30 };
            SDL_RenderCopy(renderer, explosionTextures[frame], NULL, &dstRect);
        }
    }
};
std::vector<Explosion> explosions;
SDL_Texture* explosionTextures[5]; // Ch?a 5 ?nh hi?u ?ng n?