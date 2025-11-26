#ifndef GENERATION_PARAMS_H
#define GENERATION_PARAMS_H

#include <vector>

namespace Cave {

struct GenerationStep {
    int b3_min, b3_max;
    int b5_min, b5_max;
    int s3_min, s3_max;
    int s5_min, s5_max;
    int reps;
};

struct GenerationParams {
    int seed = 0;
    int mOctaves = 8;
    bool mPerlin = false;
    float mWallChance = 0;
    float mFreq = 1;
    float mAmp = 1;
    std::vector<GenerationStep> mGenerations;
};

}

#endif
