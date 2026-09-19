#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <raylib.h>
#include <physics.h>
#include <graphics.h>
#include <random.h>
#include <constants.h>


PSP_MODULE_INFO("Watermelon Game", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define ATTR_PSP_WIDTH 480
#define ATTR_PSP_HEIGHT 272

SceCtrlData Pad;

float DeltaTime = 0.0f;
int Score = 0;
int DispScore = 0;
bool GameOver = false;

bool flag=true;
bool l1flag=false;
bool r1flag=false;
int xflag;


int main(void) {
    const int screenWidth = ATTR_PSP_WIDTH;
    const int screenHeight = ATTR_PSP_HEIGHT;

    InitWindow(screenWidth, screenHeight, "Watermelon Game");

    Texture2D fruitsTexture = LoadTexture("WatermelonGame.png");
    SetTextureFilter(fruitsTexture, TEXTURE_FILTER_BILINEAR); 

    SetTargetFPS(30);
    initRng();

    struct Ball heldBall;
    initBall(&heldBall);

    struct Ball balls [BallsLimit];
    float timeSinceLastBall = 0.5;
    int BallsN = 0;

    for (int i = 0; i < BallsLimit; i++) {
        initBall(&balls[i]);
    }

    while (flag) {
        if (GameOver) break;

        DeltaTime = (((float) GetFrameTime()) * InvTicksPerFrame);
        // TraceLog(LOG_INFO," DeltaTime = %f\n", DeltaTime);
        TraceLog(LOG_INFO," FPS = %f\n", 1.0f/(0.0000001+(float)GetFrameTime()));
        // TraceLog(LOG_INFO," Lx = %i\n", Pad.Lx);

        sceCtrlReadBufferPositive(&Pad, 1);

        if (timeSinceLastBall < 0.5) {
            timeSinceLastBall += DeltaTime * TicksPerFrame;
        } else {
            timeSinceLastBall = 0.5;
        }

        if (timeSinceLastBall >= 0.5) {
            unsigned char analogX = Pad.Lx;
            int leftPressed = ((Pad.Buttons & PSP_CTRL_LTRIGGER) || (Pad.Buttons & PSP_CTRL_LEFT) || analogX < 128 - (128*AnalogDeadzonePercent));
            int rightPressed = ((Pad.Buttons & PSP_CTRL_RTRIGGER) || (Pad.Buttons & PSP_CTRL_RIGHT) || analogX > 128 + (128*AnalogDeadzonePercent));

            float analogPercent = (fabsf((float)(128-analogX))/128);
            if ((Pad.Buttons & PSP_CTRL_LTRIGGER) || (Pad.Buttons & PSP_CTRL_LEFT) || (Pad.Buttons & PSP_CTRL_RTRIGGER) || (Pad.Buttons & PSP_CTRL_RIGHT)) {
                analogPercent = 1;
            }

            if ((Pad.Buttons & PSP_CTRL_LTRIGGER && Pad.Buttons & PSP_CTRL_RTRIGGER) || (Pad.Buttons & PSP_CTRL_CROSS) || (Pad.Buttons & PSP_CTRL_CIRCLE)) {
                timeSinceLastBall = 0;
                balls[BallsN] = heldBall;
                balls[BallsN].x += rngf(-0.01f, 0.01f);
                BallsN++;
                randomizeRadius(&heldBall);
            } else if (leftPressed && !rightPressed) {
                heldBall.x -= analogPercent * 5 * DeltaTime * TicksPerFrame;
            } else if (!leftPressed && rightPressed) {
                heldBall.x += analogPercent * 5 * DeltaTime * TicksPerFrame;
            }
        }

        if (DispScore < Score) {
            DispScore++;
        }

        resolveLeftWallColl(&heldBall, false);
        resolveRightWallColl(&heldBall, false);

        for (int tick = 0; tick < TicksPerFrame; tick++) {
            for (int i = 0; i < BallsN; i++) {
                struct Ball * b = &(balls[i]);

                struct Vector2D f = {0, 0};
                addGravToForce(&f, b);

                struct Vector2D a = calcAcc(f, b);
                addAccToVel(a, b);

                applyAirResistance(b);
                moveBall(b);

                resolveFloorColl(b);
                resolveLeftWallColl(b, true);
                resolveRightWallColl(b, true);

                if (b->y + b->r < BoxFloorY - BoxTopLineHeight) {
                    b->timeOutside += DeltaTime;
                } else {
                    b->timeOutside = 0;
                }

                if (b->timeOutside >= 5) {
                    GameOver = true;
                }
            }

            sortBalls(balls, BallsN); // Should improve collisions resolving
        
            for (int i = 0; i < BallsN; i++) {
                struct Ball * b1 = &(balls[i]);

                for (int pass = 0; pass < CollPasses; pass++) {
                    for (int j = i+1; j < BallsN; j++) {
                        struct Ball * b2 = &(balls[j]);

                        tryCombiningBalls(balls, i, j, &BallsN);
                        resolveBallColl(b1, b2);

                        resolveFloorColl(b1);
                        resolveLeftWallColl(b1, true);
                        resolveRightWallColl(b1, true);
                        resolveFloorColl(b2);
                        resolveLeftWallColl(b2, true);
                        resolveRightWallColl(b2, true);
                    }
                }
            }
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            drawBox();

            if (timeSinceLastBall >= 0.5) {
                drawBall(&heldBall, fruitsTexture);
            }

            for (int i = 0; i < BallsN; i++) {
                struct Ball * b = &(balls[i]);
                drawBall(b, fruitsTexture);
            }

            DrawText(TextFormat("Score: %08i", DispScore), 260, 16, 20, BLACK);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
