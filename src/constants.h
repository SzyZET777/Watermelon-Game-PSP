#ifndef CONSTANTS_H
#define CONSTANTS_H

static const float PixelsPerUnit = 30.0f;
static const float PPU = PixelsPerUnit;
static const float BoxTopMargin = 1.0f;
static const float BoxLeftMargin = 1.0f;
static const float BoxHeight = 7.6f;
static const float BoxTopLineHeight = 6.7f;
static const float BoxWidth = 6.5f;
static const float BoxFloorY = BoxTopMargin + BoxHeight;
static const float TextLeftMargin = 2.0;

static const float AnalogDeadzonePercent = 0.15;

static const float TimeToFall = 1.0f;
static const float Gravity = 2.0f * BoxHeight / (TimeToFall*TimeToFall);
static const float AirResistance = 4.0f;
static const float PenetrationCorrectionPercent = 0.75f;
static const float MergePushForce = 5.0f;

static const int BallsLimit = 512;
static const int CollPasses = 4;
static const int TicksPerFrame = 8;
static const float InvTicksPerFrame = 1.0f / ((float) TicksPerFrame);

static const float BallRadius [] = {0.2f,0.25f,0.45f,0.5f,0.6f,0.75f,0.85f,1.05f,1.15f,1.45,1.7};
static const Color BallColors [] = {DARKBLUE,RED,LIME,GOLD,ORANGE,MAROON,PINK,BROWN,GREEN,YELLOW,DARKGREEN};
static const int BallTypesN = sizeof(BallRadius) / sizeof(BallRadius[0]);
static const int BallScores [] = {1,3,7,9,13,21,27,34,44,62,90};

static const int FruitTextureOffsets [] = {0,16,48,80,112,176,240,304,304,304};
static const int FruitTextureWH [] = {16,32,32,32,64,64,64,128,128,128,128};

// #define DeltaTime (((float) GetFrameTime()) / TicksPerFrame)
// static const float DeltaTime = (0.0333f / TicksPerFrame);
extern float DeltaTime;
extern int Score;
extern int DispScore;
extern bool GameOver;

#endif