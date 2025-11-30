#include <iostream>
#include <vector>
#include "core/Cave.h"
#include "core/CaveInfo.h"
#include "core/GenerationParams.h"

int main() {
    Cave::Cave cave;
    Cave::CaveInfo info;
    Cave::GenerationParams params;

    // Generation parameters
    params.seed = 424242;
    params.mOctaves = 1;
    params.mPerlin = false;
    params.mWallChance = 0.65;
    params.mFreq = 13.7;

    Cave::GenerationStep step;
    step.b3_min = 3;
    step.b3_max = 4;
    step.b5_min = 12;
    step.b5_max = 16;
    step.s3_min = 2;
    step.s3_max = 5;
    step.s5_min = 10;
    step.s5_max = 14;
    step.reps = 2;
    params.mGenerations.push_back(step);

    // CaveInfo parameters
    info.mCaveWidth = 32;
    info.mCaveHeight = 32;
    info.mBorderWidth = 1;
    info.mBorderHeight = 1;
    info.mCellWidth = 8;
    info.mCellHeight = 8;
    info.mFloor = ' ';
    info.mWall = '#';
    std::vector<std::vector<int>> tileMap(info.mCaveHeight, std::vector<int>(info.mCaveWidth));
    info.pTileMap = &tileMap;

    // Generate the cave
    cave.generate(info, params);

    // Print the tile map to the console
    for (int y = 0; y < info.mCaveHeight; ++y) {
        for (int x = 0; x < info.mCaveWidth; ++x) {
            std::cout << (char)tileMap[y][x];
        }
        std::cout << std::endl;
    }

    return 0;
}
