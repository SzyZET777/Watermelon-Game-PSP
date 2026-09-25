#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <raylib.h>
#include <physics.h>
#include <graphics.h>
#include <constants.h> 


void drawBall(struct Ball * b, Texture2D fruitsTexture){
    float rInPixels = b->r * PPU;
    Rectangle source = {FruitTextureOffsets[b->typeIdx], 0.0f, FruitTextureWH[b->typeIdx], FruitTextureWH[b->typeIdx]};
    Rectangle dest = {b->x*PPU, b->y*PPU, FruitTextureWH[b->typeIdx], FruitTextureWH[b->typeIdx]};
    Vector2 origin = {rInPixels+1.0f, rInPixels+1.0f};

    if (b->y + b->r < BoxFloorY - BoxTopLineHeight && b->timeOutside >= 1) {
        float ballTintPercent = fabsf(sinf(M_PI * b->timeOutside));
        Color ballTint = RED;
        if (b->timeOutside < 3) {
            ballTint.r = WHITE.r * (1.0f - ballTintPercent) + RED.r * (ballTintPercent);
            ballTint.g = WHITE.g * (1.0f - ballTintPercent) + RED.g * (ballTintPercent);
            ballTint.b = WHITE.b * (1.0f - ballTintPercent) + RED.b * (ballTintPercent);
        }
        if (b->typeIdx > 8) {
            DrawCircle(b->x * PPU, b->y * PPU, b->r * PPU, ballTint);
            DrawCircleLines(b->x * PPU, b->y * PPU, b->r * PPU, BLACK);
        } else {
            DrawTexturePro(fruitsTexture, source, dest, origin, radToDeg(b->rot), ballTint);
        }
    } else {
        if (b->typeIdx > 8) {
            DrawCircle(b->x * PPU, b->y * PPU, b->r * PPU, BallColors[b->typeIdx]);
            DrawCircleLines(b->x * PPU, b->y * PPU, b->r * PPU, BLACK);
        } else {
            DrawTexturePro(fruitsTexture, source, dest, origin, radToDeg(b->rot), WHITE);
        }
    }
}


void drawBox() {
    DrawLine(BoxLeftMargin * PPU, BoxFloorY * PPU, (BoxLeftMargin+BoxWidth) * PPU, BoxFloorY * PPU, BLACK);
    DrawLine(BoxLeftMargin * PPU, BoxTopMargin * PPU, BoxLeftMargin * PPU, (BoxTopMargin + BoxHeight) * PPU, BLACK);
    DrawLine((BoxLeftMargin+BoxWidth) * PPU, BoxTopMargin * PPU, (BoxLeftMargin+BoxWidth) * PPU, (BoxTopMargin + BoxHeight) * PPU, BLACK);
    DrawLine(BoxLeftMargin * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, (BoxLeftMargin+BoxWidth) * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, RED);
}
