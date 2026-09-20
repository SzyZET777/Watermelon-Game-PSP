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
        if (b->typeIdx > 5) {
            DrawCircle(b->x * PPU, b->y * PPU, b->r * PPU, RED);
            DrawCircleLines(b->x * PPU, b->y * PPU, b->r * PPU, BLACK);
        } else {
            DrawTexturePro(fruitsTexture, source, dest, origin, radToDeg(b->rot), RED);
        }
    } else {
        if (b->typeIdx > 5) {
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
