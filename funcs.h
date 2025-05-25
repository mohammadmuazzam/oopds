#include "robotsAndManager.h"

int GetRandomNumber(int min, int max);

bool ProbabilityCheck(int probability);

Position GetRandomPosition(Position mapSize);

//* by default it ignores
Position GetRandomPositionCustom(Position xLimit, Position yLimit, bool ignoreZero = true);

bool IsPositionValid(const Position& pos, const Position& mapSize);

Position GetNeighborPosition(Position pos, int direction);
