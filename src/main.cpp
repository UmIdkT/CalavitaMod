#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <vita2d.h>

#include <cmath>

// First playable milestone: a small, original 2D sandbox prototype.
// This is not Terraria content or a port of Terraria's source code.

static const int SCREEN_W = 960;
static const int SCREEN_H = 544;
static const int TILE = 32;
static const int WORLD_W = 160;
static const int WORLD_H = 18;

static int groundRow[WORLD_W];

static float clampf(float value, float low, float high) {
    return value < low ? low : (value > high ? high : value);
}

static void make_world() {
    for (int x = 0; x < WORLD_W; ++x) {
        // Gentle rolling terrain, generated deterministically.
        int wave = (int)(2.0f * std::sin(x * 0.11f) + 1.5f * std::sin(x * 0.037f));
        groundRow[x] = 12 + wave;
    }
}

int main(void) {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    vita2d_init();
    make_world();

    float playerX = 180.0f;
    float playerY = 250.0f;
    float velocityY = 0.0f;
    const float playerW = 24.0f;
    const float playerH = 40.0f;
    const float gravity = 0.42f;
    const float jumpSpeed = -8.5f;
    bool grounded = false;
    bool wasJumpDown = false;
    bool running = true;

    while (running) {
        SceCtrlData pad;
        sceCtrlPeekBufferPositive(0, &pad, 1);

        if (pad.buttons & SCE_CTRL_START) {
            running = false;
        }

        float move = 0.0f;
        if (pad.buttons & SCE_CTRL_LEFT) move -= 1.0f;
        if (pad.buttons & SCE_CTRL_RIGHT) move += 1.0f;
        if (pad.lx < 90) move = -1.0f;
        if (pad.lx > 165) move = 1.0f;
        playerX += move * 3.0f;
        playerX = clampf(playerX, 0.0f, WORLD_W * TILE - playerW);

        bool jumpDown = (pad.buttons & SCE_CTRL_CROSS) != 0;
        if (jumpDown && !wasJumpDown && grounded) {
            velocityY = jumpSpeed;
            grounded = false;
        }
        wasJumpDown = jumpDown;

        velocityY += gravity;
        if (velocityY > 9.0f) velocityY = 9.0f;
        playerY += velocityY;

        int centerTile = (int)((playerX + playerW * 0.5f) / TILE);
        if (centerTile < 0) centerTile = 0;
        if (centerTile >= WORLD_W) centerTile = WORLD_W - 1;
        float floorY = (float)(groundRow[centerTile] * TILE);
        if (playerY + playerH >= floorY) {
            playerY = floorY - playerH;
            velocityY = 0.0f;
            grounded = true;
        } else {
            grounded = false;
        }

        float cameraX = clampf(playerX - SCREEN_W * 0.42f, 0.0f,
                               WORLD_W * TILE - SCREEN_W);

        vita2d_start_drawing();
        vita2d_clear_screen();

        // Sky and distant hills.
        vita2d_draw_rectangle(0, 0, SCREEN_W, SCREEN_H, RGBA8(105, 183, 225, 255));
        for (int i = 0; i < 5; ++i) {
            float hx = (float)(i * 250) - std::fmod(cameraX * 0.22f, 250.0f);
            vita2d_draw_rectangle(hx, 300, 170, 110, RGBA8(91, 157, 153, 255));
        }

        int firstTile = (int)(cameraX / TILE);
        int lastTile = firstTile + SCREEN_W / TILE + 2;
        if (lastTile >= WORLD_W) lastTile = WORLD_W - 1;

        for (int tx = firstTile; tx <= lastTile; ++tx) {
            float sx = (float)(tx * TILE) - cameraX;
            int top = groundRow[tx];

            // Grass cap.
            vita2d_draw_rectangle(sx, (float)(top * TILE), TILE, 7,
                                  RGBA8(82, 174, 74, 255));
            // Dirt below the surface.
            for (int ty = top; ty < WORLD_H; ++ty) {
                float sy = (float)(ty * TILE);
                if (ty == top) sy += 7;
                float h = (ty == top) ? 25.0f : 32.0f;
                vita2d_draw_rectangle(sx, sy, TILE, h,
                                      RGBA8(139, 91, 54, 255));
                // Subtle tile seams.
                vita2d_draw_rectangle(sx + 30, sy, 2, h,
                                      RGBA8(112, 72, 45, 255));
            }
        }

        // Simple original player placeholder.
        float drawX = playerX - cameraX;
        vita2d_draw_rectangle(drawX, playerY, playerW, playerH,
                              RGBA8(235, 190, 115, 255));
        vita2d_draw_rectangle(drawX + 5, playerY + 8, 5, 5,
                              RGBA8(35, 45, 65, 255));
        vita2d_draw_rectangle(drawX + 15, playerY + 8, 5, 5,
                              RGBA8(35, 45, 65, 255));

        // Small control legend made from clear visual markers.
        vita2d_draw_rectangle(18, 18, 150, 8, RGBA8(25, 45, 65, 180));
        vita2d_draw_rectangle(18, 18, 62, 8, RGBA8(235, 210, 125, 255));

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }

    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}
