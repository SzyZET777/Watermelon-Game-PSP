#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspfpu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <raylib.h>
#include <physics.h>
#include <graphics.h>
#include <random.h>
#include <constants.h>


float radToDeg(float rad) {
    return (rad * 180.0f) /  M_PI;
}


void initBall(struct Ball * b) {
    b->typeIdx = rng(5);
    b->r = BallRadius[b->typeIdx];
    b->m = M_PI * b->r * b->r;
    b->im = 1.0f / b->m;
    b->vx = 0.0f; 
    b->vy = 0.0f;
    b->rot = 0.0f;
    b->x = BoxLeftMargin + (BoxWidth / 2.0f);
    b->y = BoxTopMargin;
    b->timeOutside = 0.0f;
}


void randomizeRadius(struct Ball * b) {
    b->typeIdx = rng(5);
    b->r = BallRadius[b->typeIdx];
    b->m = M_PI * b->r * b->r;
    b->im = 1.0f / b->m;
}


void sortBalls(struct Ball balls[], int BallsN) {  
    for (int i = 0; i < BallsN - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < BallsN - i - 1; j++) {
            if (balls[j].y + balls[j].r < balls[j+1].y + balls[j+1].r) {
                struct Ball t = balls[j];
                balls[j] = balls[j+1];
                balls[j+1] = t;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}



void addGravToForce(struct Vector2D * f, struct Ball * b) {
    f->y += Gravity * b->m;
}


void applyAirResistance(struct Ball * b) {
    float drag = AirResistance * b->r * b->r * b->im;
    b->vx *= (1.0f - drag * DeltaTime);
    b->vy *= (1.0f - drag * DeltaTime);
}


struct Vector2D calcAcc(struct Vector2D f, struct Ball * b) {
    struct Vector2D a;
    a.x = f.x * b->im;
    a.y = f.y * b->im;
    return a;
}


void addAccToVel(struct Vector2D a, struct Ball * b) {
    b->vx += a.x * DeltaTime;
    b->vy += a.y * DeltaTime;
}


void moveBall(struct Ball * b) {
    b->x += b->vx * DeltaTime;
    b->y += b->vy * DeltaTime;
    b->rot += (b->vx * DeltaTime) / b->r;
}


void resolveFloorColl(struct Ball * b) {
    float penetrationDepth = b->y + b->r - BoxFloorY;
    if (penetrationDepth > 0) {
        b->y -= penetrationDepth;
        if (b->vy > 0) {
            b->vy = 0;
        }
    }
}


void resolveLeftWallColl(struct Ball * b, bool rotate) {
    float penetrationDepth = BoxLeftMargin - (b->x - b->r);
    if (penetrationDepth > 0) {
        b->x += penetrationDepth;
        if (b->vx < 0) {
            b->vx = 0;
        }
        if (rotate) b->rot += (penetrationDepth) / b->r;
    }
}


void resolveRightWallColl(struct Ball * b, bool rotate) {
    float penetrationDepth = b->x + b->r - (BoxLeftMargin + BoxWidth);
    if (penetrationDepth > 0) {
        b->x -= penetrationDepth;
        if (b->vx > 0) {
            b->vx = 0;
        }
        if (rotate) b->rot -= (penetrationDepth) / b->r;
    }
}


void tryCombiningBalls(struct Ball balls[], int i, int j, int * BallsN) {
    if (balls[i].typeIdx != balls[j].typeIdx) return;

    if (i > j) {
        int t = i;
        i = j;
        j = t;
    }

    float dy = balls[j].y - balls[i].y;
    float dx = balls[j].x - balls[i].x;

    float distSq = dx*dx + dy*dy;
    if (distSq == 0.0f) distSq = 0.0001;

    float dist = pspFpuSqrt(distSq);
    float penetrationDepth = balls[i].r + balls[j].r - dist;

    if (penetrationDepth > 0.0f) {
        Score += BallScores[balls[i].typeIdx];

        struct Ball b1 = balls[i];
        struct Ball b2 = balls[j];

        for (int k = 0; k < (*BallsN); k++) {
            if (k == i || k == j) continue;

            float distX = balls[k].x - ((b1.x + b2.x) / 2.0f);
            float distY = balls[k].y - ((b1.y + b2.y) / 2.0f);
            float distSq = (distX*distX)+(distY*distY);
            if (distSq == 0.0f) distSq = 0.0001;
            float totalInvDist = pspFpuRsqrt(distSq);
            float totalForce = MergePushForce * totalInvDist;
            if (totalForce > 0.25f * MergePushForce) {
                totalForce = 0.25f * MergePushForce;
            } else if (totalForce < -0.25f * MergePushForce) {
                totalForce = -0.25f * MergePushForce;
            }
            float nx = distX * totalInvDist;
            float ny = distY * totalInvDist;

            balls[k].vx += totalForce * nx;
            balls[k].vy += totalForce * ny;
        }

        if (balls[i].typeIdx == BallTypesN-1) {
            for (int k = j; k < (*BallsN)-1; k++) {
                balls[k] = balls[k+1];
            }
            (*BallsN)--;
            for (int k = i; k < (*BallsN)-1; k++) {
                balls[k] = balls[k+1];
            }
            (*BallsN)--;
        } else {
            balls[i].x = (b1.x + b2.x) / 2.0f;
            balls[i].y = (b1.y + b2.y) / 2.0f;
            balls[i].typeIdx++;
            balls[i].r = BallRadius[balls[i].typeIdx];
            balls[i].m = M_PI * balls[i].r * balls[i].r;  
            balls[i].im = 1.0f / balls[i].m;  
            balls[i].vx = 0.0f; 
            balls[i].vy = 0.0f;
            balls[i].rot = 0.0f;

            for (int k = j; k < (*BallsN)-1; k++) {
                balls[k] = balls[k+1];
            }
            (*BallsN)--;
        }
    }
}


void resolveBallColl(struct Ball * b1, struct Ball * b2) {
    float dy = b2->y-b1->y;
    float dx = b2->x-b1->x;

    float distSq = dx*dx + dy*dy;

    if (distSq == 0.0f) distSq = 0.0001;

    float dist = pspFpuSqrt(distSq);
    float invDist = pspFpuRsqrt(distSq);

    float penetrationDepth = b1->r + b2->r - dist;

    if (penetrationDepth > 0.0f) {
        float nx = dx * invDist;
        float ny = dy * invDist;

        float totalInverseMass = 1.0f / (b1->im + b2->im);
        float tmp0 = PenetrationCorrectionPercent * penetrationDepth * totalInverseMass;

        float tmp1 = tmp0 * b1->im;
        b1->x -= nx * tmp1;
        b1->y -= ny * tmp1;

        b1->rot -= (nx * tmp1) / b1->r;

        float tmp2 = tmp0 * b2->im;
        b2->x += nx * tmp2;
        b2->y += ny * tmp2;

        b2->rot += (nx * tmp2) / b2->r;

        float dvx = b2->vx - b1->vx;
        float dvy = b2->vy - b1->vy;

        float velocity = dvx * nx + dvy * ny;

        if (velocity < 0.0f) {
            float bounceVel = -velocity;
            bounceVel *= totalInverseMass;

            float tmp3 = bounceVel * b1->im;
            b1->vx -= tmp3 * nx;
            b1->vy -= tmp3 * ny;
            
            float tmp4 = bounceVel * b2->im;
            b2->vx += tmp4 * nx;
            b2->vy += tmp4 * ny;
        }
    }
}
