#include "robotsAndManager.h"

int GetRandomNumber(int min, int max);

Position AbsolutePosition(Position pos1);

bool ProbabilityCheck(int probability);

Position GetRandomPosition(Position limit);

//* by default it ignores zero
Position GetRandomPositionCustom(Position xLimit, Position yLimit, bool ignoreZero = true);
Position GetRandomPositionCustom(Position limit,  bool ignoreZero = true);

bool IsPositionValidAndUnoccupied(const Position& pos);

float GetDistance(const Position& pos1, const Position& pos2);
Position GetClosestPosition(const vector<Position>& positions);

int GetShootDistance(Position currentPosition, Position enemyPosition);

Position GetNeighborPosition(Position pos, int direction);


