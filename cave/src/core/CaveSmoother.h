#ifndef CAVE_SMOOTHER_H
#define CAVE_SMOOTHER_H

#include <vector>

#include "CaveInfo.h"

namespace Cave {

class CaveSmoother {
public:
	CaveSmoother(const CaveInfo& i);
	~CaveSmoother();

	void smoothEdges();

private:
	bool isInBounds(int x, int y);
	bool isWall(int cx, int cy);
	bool isSolid(int cx, int cy, std::vector< std::vector<int> > grid);
	Vector2i getMapPos(int x, int y);
	void setCell(int cx, int cy, Vector2i tile);

private:
	const CaveInfo& info;
};

} // namespace

#endif

