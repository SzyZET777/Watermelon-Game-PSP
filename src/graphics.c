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
    Rectangle source = {FruitTextureOffsetX[b->typeIdx], FruitTextureOffsetY[b->typeIdx], FruitTextureWH[b->typeIdx], FruitTextureWH[b->typeIdx]};
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
        // DrawCircle(b->x * PPU, b->y * PPU, b->r * PPU, ballTint);
        // DrawCircleLines(b->x * PPU, b->y * PPU, b->r * PPU, BLACK);
        DrawTexturePro(fruitsTexture, source, dest, origin, radToDeg(b->rot), ballTint);
    } else {
        // DrawCircle(b->x * PPU, b->y * PPU, b->r * PPU, BallColors[b->typeIdx]);
        // DrawCircleLines(b->x * PPU, b->y * PPU, b->r * PPU, BLACK);
        DrawTexturePro(fruitsTexture, source, dest, origin, radToDeg(b->rot), WHITE);
    }
}


void drawBox() {
    DrawRectangle(BoxLeftMargin * PPU, ((BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight)) * PPU, BoxWidth * PPU, (BoxTopLineHeight + BoxDiagonalLineHeight) * PPU, BoxColor);

    Vector2 v1 = {(BoxLeftMargin+BoxDiagonalLineHeight) * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU};
    Vector2 v2 = {BoxLeftMargin * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU};
    Vector2 v3 = {BoxLeftMargin * PPU, (BoxFloorY-BoxTopLineHeight) * PPU};
    DrawTriangle(v1, v2, v3, BackgroundColor);

    Vector2 v4 = {(BoxLeftMargin+BoxWidth) * PPU, (BoxFloorY-BoxTopLineHeight) * PPU};
    Vector2 v5 = {(BoxLeftMargin+BoxWidth) * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU};
    Vector2 v6 = {(BoxLeftMargin+BoxWidth-BoxDiagonalLineHeight) * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU};
    DrawTriangle(v4, v5, v6, BackgroundColor);

    DrawLine(BoxLeftMargin * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, (BoxLeftMargin+BoxWidth) * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, RED);

    DrawLine(BoxLeftMargin * PPU, BoxFloorY * PPU, (BoxLeftMargin+BoxWidth) * PPU, BoxFloorY * PPU, OutlineColor);
    DrawLine(BoxLeftMargin * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, BoxLeftMargin * PPU, (BoxTopMargin + BoxHeight) * PPU, OutlineColor);
    DrawLine((BoxLeftMargin+BoxWidth) * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, (BoxLeftMargin+BoxWidth) * PPU, (BoxTopMargin+BoxHeight) * PPU, OutlineColor);

    DrawLine(BoxLeftMargin * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, (BoxLeftMargin+BoxDiagonalLineHeight) * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU, OutlineColor);
    DrawLine((BoxLeftMargin+BoxWidth) * PPU, (BoxFloorY-BoxTopLineHeight) * PPU, (BoxLeftMargin+BoxWidth-BoxDiagonalLineHeight) * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU, OutlineColor);
    DrawLine((BoxLeftMargin+BoxDiagonalLineHeight) * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU, (BoxLeftMargin+BoxWidth-BoxDiagonalLineHeight) * PPU, (BoxFloorY-BoxTopLineHeight-BoxDiagonalLineHeight) * PPU, OutlineColor);
}
