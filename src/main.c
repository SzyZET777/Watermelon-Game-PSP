#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspaudio.h>
#include <psppower.h>
#include <pspmscm.h>
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

SceCtrlData Pad;

float timerStart, timerEnd;
float DeltaTime = 0.0f;
int initialBalls = 0;
int Score = 0;
int DispScore = 0;
int MusicTrack = 0;
int AudioVolume = 64;
bool GameOver = false;
bool SelectPressed = false;

const int quickSleep = 10000;
const int longSleep = 10000;

volatile bool SleepMode = false;
volatile bool SleepModeStart = false;
volatile bool SleepModeStarted = false;
volatile bool SleepModeResume = false;


int MyPowerCallback(int unknown, int pwrflags, void *common) {
    if (pwrflags & (PSP_POWER_CB_SUSPENDING | PSP_POWER_CB_STANDBY)) {
        SleepModeStarted = false;
        SleepModeStart = true;
        while (!SleepModeStarted) {
            sceKernelDelayThread(quickSleep);
        }
    }
    if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE) {
        SleepModeResume = true;
    }
    return 0;
}


int MyCallbackThread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Power Callback", MyPowerCallback, NULL);
    int result = scePowerRegisterCallback(0, cbid);
    TraceLog(LOG_INFO, "Power registration: %08X\n", result);
    sceKernelSleepThreadCB(); 
    return 0;
}


void MySetupCallbacks(void) {
    int thid = sceKernelCreateThread("update_thread", MyCallbackThread, 0x11, 0x4000, 0, 0);
    if (thid >= 0) sceKernelStartThread(thid, 0, 0);
}


int main(void) {
    MySetupCallbacks();

    const int screenWidth = ATTR_PSP_WIDTH;
    const int screenHeight = ATTR_PSP_HEIGHT;

    InitWindow(screenWidth, screenHeight, "Watermelon Game");
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048);
    
    Mix_Chunk* popSfx = Mix_LoadWAV("Resources/Pop.wav");
    Mix_Chunk* whooshSfx = Mix_LoadWAV("Resources/Whoosh.wav");

    Mix_Music *firstDropMusic = Mix_LoadMUS("Resources/Watermelon Game - First Drop.mp3");
    Mix_Music *smallFruitsMusic = Mix_LoadMUS("Resources/Watermelon Game - Small Fruits.mp3");

    Mix_VolumeMusic(AudioVolume);
    Mix_PlayMusic(firstDropMusic, 0);
    MusicTrack = 1;
    
    Texture2D fruitsTexture = LoadTexture("Resources/WatermelonGame.png");
    SetTextureFilter(fruitsTexture, TEXTURE_FILTER_BILINEAR);

    Font gameFont = LoadFontEx("resources/font.otf", 20, NULL, 0);

    {
        FILE* highscoreFile = fopen("highscore.txt", "rb+");
        if (highscoreFile == NULL) {
            highscoreFile = fopen("highscore.txt", "wb+");
            if (highscoreFile == NULL) {
                TraceLog(LOG_INFO, "Can't create \"highscore.txt\" file");
                CloseWindow();
                return 0;
            }
            fclose(highscoreFile);
            highscoreFile = fopen("highscore.txt", "rb+");
        }
        int highscore = 0;
        int freadCnt = fread(&highscore, sizeof(highscore), 1, highscoreFile);
        if (freadCnt == 0) {
            fseek(highscoreFile, 0, SEEK_SET);
            fwrite(&highscore, sizeof(highscore), 1, highscoreFile);
        }
        fclose(highscoreFile);
    }

    SetTargetFPS(60);
    initRng();

    struct Ball heldBall, nextBall;
    struct Ball balls [BallsLimit];
    float timeSinceLastBall = 0.5;
    int BallsN = 0;

    initBall(&heldBall);
    initBall(&nextBall);
    for (int i = 0; i < BallsLimit; i++) {
        initBall(&balls[i]);
    }

    for (int i = 0; i < initialBalls; i++) {
        initBall(&balls[i]);
        balls[BallsN] = heldBall;
        balls[BallsN].x = rngf(BoxLeftMargin, BoxLeftMargin+BoxWidth);
        BallsN++;
        randomizeRadius(&heldBall);
    }

    while (!WindowShouldClose()) {
        if (SleepModeStart) {
            scePowerLock(0);

            SleepModeStart = false;
            SleepMode = true;

            Mix_HaltMusic();
            Mix_HaltChannel(-1);

            Mix_FreeChunk(popSfx);
            Mix_FreeChunk(whooshSfx);
            Mix_FreeMusic(firstDropMusic);
            Mix_FreeMusic(smallFruitsMusic);

            popSfx = NULL;
            whooshSfx = NULL;
            firstDropMusic = NULL;
            smallFruitsMusic = NULL;

            Mix_CloseAudio();

            sceKernelDelayThreadCB(longSleep); 
            SleepModeStarted = true;

            scePowerUnlock(0);
        }

        if (SleepModeResume) {
            SleepModeResume = false;
            SleepMode = false;

            scePowerTick(0);
            sceKernelDelayThread(longSleep);
            Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048);

            while (!(popSfx && whooshSfx && firstDropMusic && smallFruitsMusic)) {
                popSfx = Mix_LoadWAV("Resources/Pop.wav");
                whooshSfx = Mix_LoadWAV("Resources/Whoosh.wav");

                firstDropMusic = Mix_LoadMUS("Resources/Watermelon Game - First Drop.mp3");
                smallFruitsMusic = Mix_LoadMUS("Resources/Watermelon Game - Small Fruits.mp3");
            
                if (!(popSfx && whooshSfx && firstDropMusic && smallFruitsMusic)) {
                    Mix_FreeChunk(popSfx);
                    Mix_FreeChunk(whooshSfx);
                    Mix_FreeMusic(firstDropMusic);
                    Mix_FreeMusic(smallFruitsMusic);
                    popSfx = NULL;
                    whooshSfx = NULL;
                    firstDropMusic = NULL;
                    smallFruitsMusic = NULL;
                    sceKernelDelayThread(longSleep);
                }
            }

            if (MusicTrack == 1) {
                Mix_PlayMusic(firstDropMusic, 0);
            } else if (MusicTrack == 2) {
                Mix_PlayMusic(smallFruitsMusic, 0);
            }
        }

        if (SleepMode) {
            sceKernelDelayThreadCB(quickSleep); 
            continue;
        }

        scePowerTick(0);

        timerStart = sceKernelGetSystemTimeLow();

        if (Mix_PlayingMusic() == 0 && MusicTrack == 1) {
            Mix_PlayMusic(smallFruitsMusic, 0);
            MusicTrack = 2;
        } else if (Mix_PlayingMusic() == 0 && MusicTrack == 2) {
            Mix_PlayMusic(firstDropMusic, 0);
            MusicTrack = 1;
        }

        DeltaTime = ((float) GetFrameTime()) * InvTicksPerFrame;

        if (DeltaTime > (1.0f / 10.0f) * InvTicksPerFrame) {
            DeltaTime = (1.0f / 10.0f) * InvTicksPerFrame;
        }

        // TraceLog(LOG_INFO," DeltaTime = %f\n", DeltaTime);
        // TraceLog(LOG_INFO," FPS = %f\n", 1.0f/(0.0000001+(float)GetFrameTime()));

        sceCtrlPeekBufferPositive(&Pad, 1);

        if ((Pad.Buttons & PSP_CTRL_SELECT) && !SelectPressed) {
            SelectPressed = true;
            if (AudioVolume == 0) {
                AudioVolume = 64;
            } else if (AudioVolume == 64) {
                AudioVolume = 128;
            } else {
                AudioVolume = 0;
            }
            Mix_VolumeMusic(AudioVolume);
        } else if (!(Pad.Buttons & PSP_CTRL_SELECT)) {
            SelectPressed = false;
        }

        if (GameOver) {
            if (Pad.Buttons & PSP_CTRL_START) {
                timeSinceLastBall = 0.5;
                BallsN = 0;
                initBall(&heldBall);
                initBall(&nextBall);
                for (int i = 0; i < BallsLimit; i++) {
                    initBall(&balls[i]);
                }
                Score = 0;
                DispScore = 0;
                GameOver = false;
            }
        } else {
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
                    Mix_PlayChannel(-1, whooshSfx, 0);
                    timeSinceLastBall = 0;
                    balls[BallsN] = heldBall;
                    balls[BallsN].x += rngf(-0.01f, 0.01f);
                    BallsN++;
                    nextBall.x = heldBall.x;
                    nextBall.y = heldBall.y;
                    heldBall = nextBall;
                    randomizeRadius(&nextBall);
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

                    if (b->timeOutside >= 3) {
                        GameOver = true;
                        FILE* highscoreFile = fopen("highscore.txt", "rb+");
                        int highscore = 0;
                        fread(&highscore, sizeof(highscore), 1, highscoreFile);
                        if (Score > highscore) {
                            fseek(highscoreFile, 0, SEEK_SET);
                            fwrite(&Score, sizeof(Score), 1, highscoreFile);
                        }
                        fclose(highscoreFile);
                    }
                }

                for (int i = 0; i < BallsN; i++) {
                    for (int j = i+1; j < BallsN; j++) {
                        if (tryCombiningBalls(balls, i, j, &BallsN)) {
                            Mix_PlayChannel(-1, popSfx, 0);
                        }
                    }
                }
                
                // It can improve physics at a cost of performance
                sortBalls(balls, BallsN);
            
                for (int pass = 0; pass < CollPasses; pass++) {
                    for (int i = 0; i < BallsN; i++) {
                        struct Ball * b1 = &(balls[i]);
                        for (int j = i+1; j < BallsN; j++) {
                            struct Ball * b2 = &(balls[j]);

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
        }

        timerEnd = sceKernelGetSystemTimeLow();
        TraceLog(LOG_INFO," Balls %i; Time before drawing: %f / %f\n", BallsN, (timerEnd-timerStart) / 1000.0f, 1000.0f * GetFrameTime());

        BeginDrawing();
        {
            ClearBackground(BackgroundColor);
            DrawRectangle(0, screenHeight-30, screenWidth, 20, FloorTopColor);
            DrawRectangle(0, screenHeight-10, screenWidth, 10, FloorBottomColor);

            drawBox();

            // DrawText("Next", 390, 16, 20, OutlineColor);
            DrawTextEx(gameFont, "Next", (Vector2){393,16}, 20.0, 2, OutlineColor);
            nextBall.x = BoxLeftMargin + BoxWidth + 2.51f;
            nextBall.y = BoxTopMargin + 0.4f + nextBall.r;
            drawBall(&nextBall, fruitsTexture);

            if (timeSinceLastBall >= 0.5) {
                drawBall(&heldBall, fruitsTexture);
            }

            for (int i = 0; i < BallsN; i++) {
                struct Ball * b = &(balls[i]);
                drawBall(b, fruitsTexture);
            }

            int highscore = 0;
            FILE* highscoreFile = fopen("highscore.txt", "rb");
            fread(&highscore, sizeof(highscore), 1, highscoreFile);
            fclose(highscoreFile);

            // DrawText(TextFormat("Score\n %08i", DispScore), 16, 16, 20, OutlineColor);
            // DrawText(TextFormat("\n\nBest\n %08i", highscore), 16, 16, 20, BROWN);
            DrawTextEx(gameFont, TextFormat("Score:\n %08i", DispScore), (Vector2){16,16}, 20.0, 2, OutlineColor);
            DrawTextEx(gameFont, TextFormat("\n\nBest:\n %08i", highscore), (Vector2){16,16}, 20.0, 2, BROWN);

            if (GameOver) {
                // DrawText("\n\n\n\n\nGame Over\nPress\n(START)\nto restart", 16, 16, 20, RED);
                DrawTextEx(gameFont, "\n\n\n\n\nGame Over\nPress START\nto restart", (Vector2){16,16}, 20.0, 2, RED);
            }
        }
        EndDrawing();
    }

    Mix_HaltMusic();
    Mix_HaltChannel(-1);

    Mix_FreeChunk(popSfx);
    Mix_FreeChunk(whooshSfx);
    Mix_FreeMusic(firstDropMusic);
    Mix_FreeMusic(smallFruitsMusic);

    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    
    CloseWindow();

    return 0;
}
