#pragma once
#include"tank.h"
struct EnemyTank {
    int x, y, speed;
    double angle; // Góc quay
    SDL_Rect rect;
    Direction direction;
    bool alive;
    std::vector<Bullet> bullets;
    Uint32 lastShotTime;
    Uint32 lastChangeTime;
    Uint32 changeInterval;
    Mix_Chunk* explosionSound = Mix_LoadWAV("C:\\Users\\ACER\\Downloads\\boom.wav");      // Âm thanh nổ
    EnemyTank(int startX, int startY) {
        x = startX;
        y = startY;
        speed = 2;
        rect = { x, y, 40, 40 };
        direction = static_cast<Direction>(rand() % 4);
        angle = getAngleFromDirection(direction);
        alive = true;
        lastShotTime = SDL_GetTicks();
        lastChangeTime = SDL_GetTicks();
        changeInterval = 1000 + rand() % 2000;
    }

    void update(std::vector<Wall>& walls, Tank& player, std::vector<EnemyTank>& enemies) {
        Uint32 currentTime = SDL_GetTicks();
        //Tim duong di den bosss
        if (currentTime - lastChangeTime >= changeInterval) {
            int dx = 380 - x;
            int dy = 380 - y;

            if (abs(dx) > abs(dy)) {
                direction = (dx > 0) ? RIGHT : LEFT;
            }
            else {
                direction = (dy > 0) ? DOWN : UP;
            }

            angle = getAngleFromDirection(direction);
            lastChangeTime = currentTime;
            changeInterval = 1000 + rand() % 1500;
        }

        if (alive) {
            int newX = x, newY = y;
            switch (direction) {
            case UP: newY -= speed; break;
            case DOWN: newY += speed; break;
            case LEFT: newX -= speed; break;
            case RIGHT: newX += speed; break;
            }

            SDL_Rect newRect = { newX + 5, newY + 5, 28,28 };

            if (newX < 0 || newX + 40 > 800 || newY < 0 || newY + 40 > 800) {
                direction = getNewDirection(direction);
                angle = getAngleFromDirection(direction);
            }
            else {
                bool collided = false;
                //kiem tra va cham voi tuong
                for (auto& wall : walls) {
                    if (SDL_HasIntersection(&newRect, &wall.rect)) {
                        direction = getNewDirection(direction);
                        angle = getAngleFromDirection(direction);
                        collided = true;
                        break;
                    }
                }
                for (auto& wall : wall2s) {
                    if (SDL_HasIntersection(&newRect, &wall.rect)) {
                        bullets.emplace_back(x, y, direction);
                        direction = getNewDirection(direction);
                        angle = getAngleFromDirection(direction);

                        collided = true;
                        break;
                    }
                }

                //kiem tra va cham voi boss
                if (SDL_HasIntersection(&newRect, &boss.rect)) {
                    direction = getNewDirection(direction);
                    angle = getAngleFromDirection(direction);
                    collided = true;
                }
                SDL_Rect newRect = { newX , newY , 40,40 };

                //Kiem tra va cham voi enemies khac
                for (auto& enemy : enemies) {
                    if (&enemy != this && SDL_HasIntersection(&newRect, &enemy.rect)) {
                        direction = getNewDirection(direction);
                        angle = getAngleFromDirection(direction);
                        collided = true;
                        break;
                    }
                }
                //kiem tra va cham voi tank
                if (SDL_HasIntersection(&newRect, &player.rect)) {
                    direction = getNewDirection(direction);
                    angle = getAngleFromDirection(direction);
                    collided = true;
                }

                if (!collided) {
                    x = newX;
                    y = newY;
                    rect.x = x;
                    rect.y = y;
                    if ((x > 360 && x < 440) || (y > 360 && y < 440))
                        if (rand() % 50 == 0) {
                            bullets.emplace_back(x, y, direction);
                        }
                }
            }

            if (rand() % 70 == 0) {
                bullets.emplace_back(x, y, direction);
            }
        }

        for (auto& bullet : bullets) bullet.update(walls);
        //kiểm tra va chạm giữa đạn và tường 2;
        for (Wall2& wall2 : wall2s) wall2.checkBulletCollision(bullets, wall2s);
        //kiem tra bắn trúng boss
        boss.checkBulletCollision(bullets);
        // 🚀 **Kiểm tra va chạm giữa đạn của EnemyTank và Tank**
        for (auto& enemyBullet : bullets) {
            if (!enemyBullet.active) continue;

            for (auto& playerBullet : player.bullets) {
                if (!playerBullet.active) continue;

                if (SDL_HasIntersection(&enemyBullet.rect, &playerBullet.rect)) {
                    enemyBullet.active = false;
                    playerBullet.active = false;

                    // Căn chỉnh vị trí vụ nổ vào chính giữa viên đạn
                    int explosionX = (enemyBullet.rect.x + enemyBullet.rect.w / 2 +
                        playerBullet.rect.x + playerBullet.rect.w / 2) / 2 - 20; // 20 là nửa kích thước vụ nổ
                    int explosionY = (enemyBullet.rect.y + enemyBullet.rect.h / 2 +
                        playerBullet.rect.y + playerBullet.rect.h / 2) / 2 - 20;

                    explosions.emplace_back(explosionX, explosionY);
                }
            }
        }

        // 🗑️ **Xóa các viên đạn không còn hoạt động**
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());

        player.bullets.erase(std::remove_if(player.bullets.begin(), player.bullets.end(),
            [](const Bullet& b) { return !b.active; }), player.bullets.end());
        //kiểm tra đạn va chạm với tank
        for (auto& bullet : bullets) {
            if (bullet.active && SDL_HasIntersection(&bullet.rect, &player.rect)) {
                player.lives--;
                bullet.active = false;

                // Phát âm thanh ngay khi vụ nổ bắt đầu
                if (explosionSound) {
                    Mix_PlayChannel(-1, explosionSound, 0);
                }
                explosions.emplace_back(player.x, player.y); // Thêm hiệu ứng nổ
                player.x = 400;
                player.y = 680;

                if (player.lives <= 0) {
                    gameOver = 1;
                    return;
                }
            }
        }

        for (auto& bullet : player.bullets) {
            if (SDL_HasIntersection(&rect, &bullet.rect) && bullet.active) {
                if (explosionSound) {
                    Mix_PlayChannel(-1, explosionSound, 0);
                }
                alive = false;
                score++;
                explosions.emplace_back(x, y); // Hiệu ứng nổ khi EnemyTank bị bắn trúng
                if (score > maxScore) maxScore = score;
                bullet.active = false;
                return;
            }
        }

    }

    Direction getNewDirection(Direction oldDirection) {
        Direction newDir;
        do {
            newDir = static_cast<Direction>(rand() % 4);
        } while (newDir == oldDirection);
        return newDir;
    }

    double getAngleFromDirection(Direction dir) {
        switch (dir) {
        case UP: return 0;
        case RIGHT: return 90;
        case DOWN: return 180;
        case LEFT: return 270;
        }
        return 0;
    }

    void render() {
        if (alive) {
            SDL_RenderCopyEx(renderer, enemyTankTexture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
        }
        for (auto& bullet : bullets) bullet.render(renderer, bulletTexture);
    }
};
void Tank::update(std::vector<Wall>& walls, std::vector<Wall2>& wall2s, Boss& boss, std::vector<EnemyTank>& enemies) {
    int newX = x, newY = y;

    // Xử lý di chuyển liên tục
    if (keys[0]) newY -= speed; // UP
    if (keys[1]) newY += speed; // DOWN
    if (keys[2]) newX -= speed; // LEFT
    if (keys[3]) newX += speed; // RIGHT

    // Giữ trong màn hình
    if (newX < 0) newX = 0;
    if (newX > 800 - 40) newX = 800 - 40;
    if (newY < 0) newY = 0;
    if (newY > 800 - 40) newY = 800 - 40;

    bool canMove = true;
    SDL_Rect newRect = { newX, newY, 40,40 };

    // Kiểm tra va chạm đạn với tường 2
    for (Wall2& wall2 : wall2s) {
        wall2.checkBulletCollision(bullets, wall2s);
    }

    // Kiểm tra đạn trúng boss
    boss.checkBulletCollision(bullets);

    // Kiểm tra va chạm với xe địch
    for (auto& enemy : enemies) {
        if (SDL_HasIntersection(&newRect, &enemy.rect)) return; // Không di chuyển nếu chạm xe địch
    }

    // Kiểm tra va chạm với tường
    newRect = { newX + 5, newY + 5, 28,28 };
    for (auto& wall : walls) {
        if (SDL_HasIntersection(&newRect, &wall.rect)) {
            canMove = false;
            break;
        }
    }
    for (auto& wall : wall2s) {
        if (SDL_HasIntersection(&newRect, &wall.rect)) {
            canMove = false;
            break;
        }
    }

    // Kiểm tra va chạm với boss
    if (SDL_HasIntersection(&newRect, &boss.rect)&&!mode_2) {
        canMove = false;
    }

    if (canMove) {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
    }

    // Cập nhật đạn
    for (auto& bullet : bullets) {
        bullet.update(walls);
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
}
