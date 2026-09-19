#ifndef PHYSICS_H
#define PHYSICS_H

struct Vector2D {
    float x, y;
};

struct Ball {
    float x, y;
    float vx, vy;
    float r, m, im;
    float rot;
    int typeIdx;
    float timeOutside;
};


float radToDeg(float rad);
float fastSqrtApprox(float x);
float fastInvSqrtApprox(float number);
void initBall(struct Ball * b);
void randomizeRadius(struct Ball * b);
void sortBalls(struct Ball balls[], int BallsN);
void addGravToForce(struct Vector2D * f, struct Ball * b);
void applyAirResistance(struct Ball * b);
struct Vector2D calcAcc(struct Vector2D f, struct Ball * b);
void addAccToVel(struct Vector2D a, struct Ball * b);
void moveBall(struct Ball * b);
void resolveFloorColl(struct Ball * b);
void resolveLeftWallColl(struct Ball * b, bool rotate);
void resolveRightWallColl(struct Ball * b, bool rotate);
void tryCombiningBalls(struct Ball balls[], int i, int j, int * BallsN);
void resolveBallColl(struct Ball * b1, struct Ball * b2);

#endif