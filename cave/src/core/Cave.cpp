#include "Cave.h"
#include "CaveSmoother.h"
#include "RogueCave.hpp"
#include "PerlinNoise.h"
#include "SimplexNoise.h"
#include "RandSimple.h"
#include "DJSets.h"
#include "DisjointSets.h"
#include <sstream>
#include <set>
#include <algorithm>
#include <functional>
#include "MathStuff.h"
#include <cmath>

namespace Cave {

Cave::Cave() {}
Cave::~Cave() {}

void Cave::generate(CaveInfo& info, const GenerationParams& params) {
    initialise(info, params);
    runCellularAutomata(info, params);
    fixUp(info);
    auto floorMaps = findRooms(info);
    joinRooms(info, floorMaps);
    smooth(info);
}

void Cave::initialise(CaveInfo& info, const GenerationParams& params) {
    RNG::RandSimple simple(params.seed);

    for (int cy = 0; cy < info.mCaveHeight; ++cy) {
        for (int cx = 0; cx < info.mCaveWidth; ++cx) {
            double x = cx / (info.mCaveWidth - 1 + params.mAmp) * params.mFreq;
            double y = cy / (info.mCaveHeight - 1 + params.mAmp) * params.mFreq;
            double n1 = params.mPerlin ? Algo::getSNoise2(x, y, params.mOctaves) : std::fabs(simple.getFloat()) - params.mWallChance;
            if (n1 < 0) {
                (*info.pTileMap)[cy][cx] = info.mWall;
            } else {
                (*info.pTileMap)[cy][cx] = info.mFloor;
            }
        }
    }
}

void Cave::runCellularAutomata(CaveInfo& info, const GenerationParams& params) {
    if (!params.mGenerations.empty()) {
        PCG::RogueCave cave(info.mCaveWidth, info.mCaveHeight);
        std::vector<std::vector<int>>& gridIn = cave.getGrid();
        for (int cy = 0; cy < info.mCaveHeight; ++cy) {
            for (int cx = 0; cx < info.mCaveWidth; ++cx) {
                gridIn[cy][cx] = ((*info.pTileMap)[cy][cx] == info.mWall) ? PCG::RogueCave::TILE_WALL : PCG::RogueCave::TILE_FLOOR;
            }
        }

        for (const auto& gen : params.mGenerations) {
            Util::IntRange b3(gen.b3_min, gen.b3_max);
            Util::IntRange b5(gen.b5_min, gen.b5_max);
            Util::IntRange s3(gen.s3_min, gen.s3_max);
            Util::IntRange s5(gen.s5_min, gen.s5_max);
            cave.addGeneration(b3, b5, s3, s5, gen.reps);
        }

        std::vector<std::vector<int>>& gridOut = cave.generate();
        for (int cy = 0; cy < info.mCaveHeight; ++cy) {
            for (int cx = 0; cx < info.mCaveWidth; ++cx) {
                (*info.pTileMap)[cy][cx] = (gridOut[cy][cx] == PCG::RogueCave::TILE_WALL) ? info.mWall : info.mFloor;
            }
        }
    }
}

void Cave::fixUp(CaveInfo& info) {
	std::vector<Vector2i> walls;
	std::vector<Vector2i> floors;
	for (int lp=0; lp < 10; ++lp) {
		for (int cy=0; cy < info.mCaveHeight; ++cy) {
			for (int cx=0; cx < info.mCaveWidth; ++cx) {
				int DIAG = 0;
				int NSEW = 0;

				if (isWall(cx - 1, cy - 1, info)) DIAG += 1;
				if (isWall(cx, cy - 1, info)) NSEW += 1;
				if (isWall(cx + 1, cy - 1, info)) DIAG += 2;
				if (isWall(cx + 1, cy, info)) NSEW += 2;
				if (isWall(cx - 1, cy, info)) NSEW += 8;
				if (isWall(cx - 1, cy + 1, info)) DIAG += 8;
				if (isWall(cx, cy + 1, info)) NSEW += 4;
				if (isWall(cx + 1, cy + 1, info)) DIAG += 4;

				if (isWall(cx, cy, info)) {
					if (((DIAG&0b0001) != 0) && ((NSEW&0b1001) == 0)) floors.push_back({cx, cy});
					if (((DIAG&0b0010) != 0) && ((NSEW&0b0011) == 0)) floors.push_back({cx, cy});
					if (((DIAG&0b0100) != 0) && ((NSEW&0b0110) == 0)) floors.push_back({cx, cy});
					if (((DIAG&0b1000) != 0) && ((NSEW&0b1100) == 0)) floors.push_back({cx, cy});
				} else {
					if ((DIAG == 0b1111) && (NSEW == 0b1111)) walls.push_back({cx, cy});
				}
			}
		}
		if (walls.empty() && floors.empty()) return;
		for (Vector2i corner : walls) setCell(info, corner, info.mWall);
		for (Vector2i corner : floors) setCell(info, corner, info.mFloor);
		walls.clear();
		floors.clear();
	}
}

std::pair<Vector2iIntMap, IntVectorOfVector2iMap> Cave::findRooms(CaveInfo& info) {
    Algo::DisjointSets<Vector2i> floors;
    static const std::vector<Vector2i> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    for (int cx = 0; cx < info.mCaveWidth; ++cx) {
        for (int cy = 0; cy < info.mCaveHeight; ++cy) {
            if (isFloor(cx, cy, info)) {
                floors.addElement({cx,cy});
            }
        }
    }

    for (int cx = 0; cx < info.mCaveWidth; ++cx) {
        for (int cy = 0; cy < info.mCaveHeight; ++cy) {
            if (isFloor(cx, cy, info)) {
                for (const Vector2i& dir : directions) {
                    int nx = cx + dir.x;
                    int ny = cy + dir.y;
                    if ((nx >= 0 && nx < info.mCaveWidth) && (ny >= 0 && ny < info.mCaveHeight)) {
                        if (isFloor(nx, ny, info)) {
                            int i1 = floors.findSet({cx,cy});
                            int i2 = floors.findSet({nx,ny});
                            floors.joinSets(i1, i2);
                        }
                    }
                }
            }
        }
    }

    Vector2iIntMap grid_to_set;
    IntVectorOfVector2iMap set_to_cells;

    for (int cx = 0; cx < info.mCaveWidth; ++cx) {
        for (int cy = 0; cy < info.mCaveHeight; ++cy) {
            if (isFloor(cx, cy, info)) {
                Vector2i current = {cx, cy};
                int rootId = floors.findSet(current);
                grid_to_set[current] = rootId;
                set_to_cells[rootId].push_back(current);
            }
        }
    }

    return std::pair(grid_to_set, set_to_cells);
}

void Cave::joinRooms(CaveInfo& info, std::pair<Vector2iIntMap, IntVectorOfVector2iMap> floorMaps) {
    std::vector<Cave::BorderWall> borderWalls = detectBorderWalls(info, floorMaps);
    IntVectorOfVector2iMap roomToFloorsMap = floorMaps.second;

    std::vector<int> roomIds;
    for (auto p : roomToFloorsMap) {
        roomIds.push_back(p.first);
    }
    std::vector<Cave::BorderWall> mst = findMST_Kruskal(borderWalls, roomIds);
    for (auto& node : mst) {
        int wx = node.floor1.x + node.dir.x;
        int wy = node.floor1.y + node.dir.y;
        for (int i = 0; i < node.thickness; ++i) {
            setCell(info, {wx, wy}, info.mFloor);
            wx += node.dir.x;
            wy += node.dir.y;
        }
    }
}

std::vector<Cave::BorderWall> Cave::detectBorderWalls(CaveInfo& info, std::pair<Vector2iIntMap, IntVectorOfVector2iMap> floorMaps) {
    std::vector<BorderWall> borderWalls;
    Vector2iIntMap floorToRoomMap = floorMaps.first;
    IntVectorOfVector2iMap roomsMap = floorMaps.second;

    std::vector<int> checkedRooms;

    for (const auto& [roomID, tiles] : roomsMap) {
        checkedRooms.push_back(roomID);
        for (const auto& tile : tiles) {
            for (const auto& dir : {Vector2i{-1, 0}, Vector2i{1, 0}, Vector2i{0, -1}, Vector2i{0, 1}}) {
                int cx = tile.x + dir.x;
                int cy = tile.y + dir.y;
                if (isWall(cx, cy, info)) {
                    std::set<int> adjacentRooms;
                    adjacentRooms.insert(roomID);

                    int thickness = 0;
                    while (isWall(cx, cy, info)) {
                        cx += dir.x;
                        cy += dir.y;
                        thickness++;
                    }

                    if (isFloor(cx, cy, info)) {
                        auto it = floorToRoomMap.find({cx, cy});
                        if (it != floorToRoomMap.end()) {
                            int otherRoomID = it->second;
                            if (std::find(checkedRooms.begin(), checkedRooms.end(), otherRoomID) == checkedRooms.end()) {
                                adjacentRooms.insert(otherRoomID);
                            }
                        }
                    }

                    if (adjacentRooms.size() == 2) {
                        auto it = adjacentRooms.begin();
                        int room1 = *it++;
                        int room2 = *it;
                        borderWalls.push_back({tile, {cx, cy}, dir, room1, room2, thickness});
                    }
                }
            }
        }
    }
    return borderWalls;
}

std::vector<Cave::BorderWall> Cave::findMST_Kruskal(std::vector<Cave::BorderWall>& borderWalls, std::vector<int> roomIds) {
    std::vector<BorderWall> mst;
    Algo::DisjointSets<int> dsu;
    const int numRooms = roomIds.size();

    for (int i : roomIds) {
        dsu.addElement(i);
    }

    std::sort(borderWalls.begin(), borderWalls.end(), [](const BorderWall& a, const BorderWall& b) {
        return a.thickness < b.thickness;
    });

    for (const BorderWall& wall : borderWalls) {
        int setU = dsu.findSet(wall.room1);
        int setV = dsu.findSet(wall.room2);
        if (setU != setV) {
            mst.push_back(wall);
            dsu.joinSets(setU, setV);
        }
        if (mst.size() == numRooms - 1) break;
    }
    return mst;
}

void Cave::smooth(CaveInfo& info) {
    CaveSmoother smoother(info);
    smoother.smoothEdges();
}

bool Cave::isWall(int cx, int cy, CaveInfo& info) {
    if (cx >= 0 && cx < info.mCaveWidth && cy >= 0 && cy < info.mCaveHeight) {
        return (*info.pTileMap)[cy][cx] == info.mWall;
    }
    return false;
}

bool Cave::isFloor(int cx, int cy, CaveInfo& info) {
    if (cx >= 0 && cx < info.mCaveWidth && cy >= 0 && cy < info.mCaveHeight) {
        return (*info.pTileMap)[cy][cx] == info.mFloor;
    }
    return false;
}

void Cave::setCell(CaveInfo& info, Vector2i coords, int tile) {
    (*info.pTileMap)[coords.y][coords.x] = tile;
}

Vector2i Cave::getMapPos(CaveInfo& info, int x, int y) {
    return {info.mBorderWidth + x * info.mCellWidth, info.mBorderHeight + y * info.mCellHeight};
}

} // namespace Cave
