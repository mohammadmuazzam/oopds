#include "robotsAndManager.h"

int GetRandomNumber(int min, int max);

bool ProbabilityCheck(int probability);

Position GetRandomPosition(Position limit);

//* by default it ignores
Position GetRandomPositionCustom(Position xLimit, Position yLimit, bool ignoreZero = true);
Position GetRandomPositionCustom(Position limit,  bool ignoreZero = true);

bool IsPositionValidOrOccupied(const Position& pos);

Position GetNeighborPosition(Position pos, int direction);
