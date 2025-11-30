#ifndef CAVE_H
#define CAVE_H

#include <vector>
#include <map>
#include <unordered_map>
#include <cstddef>
#include "CaveInfo.h"
#include "GenerationParams.h"

namespace Cave {

    struct Vector2Hash {
        size_t operator()(const Vector2i& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };

    struct Vector2Equal {
        bool operator()(const Vector2i& lhs, const Vector2i& rhs) const {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };

    using Vector2iIntMap = std::unordered_map<Vector2i, int, Vector2Hash, Vector2Equal>;
	using IntVectorOfVector2iMap = std::unordered_map<int, std::vector<Vector2i>> ;

class Cave {
public:
    Cave();
    ~Cave();

    void generate(CaveInfo& info, const GenerationParams& params);

private:
    void initialise(CaveInfo& info, const GenerationParams& params);
    void runCellularAutomata(CaveInfo& info, const GenerationParams& params);
	void fixUp(CaveInfo& info);
    std::pair< Vector2iIntMap, IntVectorOfVector2iMap > findRooms(CaveInfo& info);
    void joinRooms(CaveInfo& info, std::pair<Vector2iIntMap, IntVectorOfVector2iMap> floorMaps);
    void smooth(CaveInfo& info);

	struct BorderWall {
		Vector2i floor1;
		Vector2i floor2;
		Vector2i dir;
		int room1;
		int room2;
		int thickness;
	};
	std::vector<BorderWall> detectBorderWalls(CaveInfo& info, std::pair<Vector2iIntMap, IntVectorOfVector2iMap> floorMaps);
	std::vector<BorderWall> findMST_Kruskal(std::vector<Cave::BorderWall>& borderWalls, std::vector<int> roomIds);

	bool isWall(int cx, int cy, CaveInfo& info);
	bool isFloor(int cx, int cy, CaveInfo& info);
	void setCell(CaveInfo& info, Vector2i coords, int tile);
	Vector2i getMapPos(CaveInfo& info, int x, int y);
};

} // namespace Cave

#endif
