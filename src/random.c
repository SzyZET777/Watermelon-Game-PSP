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
#include <random.h>
#include <constants.h>


SceKernelUtilsMt19937Context rngContext;

void initRng() {
    int rngSeed = sceKernelGetSystemTimeLow();
    sceKernelUtilsMt19937Init(&rngContext, rngSeed);
}

int rng(int n) {
    return sceKernelUtilsMt19937UInt(&rngContext) % n;
}

float rngf(float minf, float maxf) {
    return minf + (maxf - minf) * (((float)sceKernelUtilsMt19937UInt(&rngContext)) / INT32_MAX);
}
