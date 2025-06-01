#include "robotsAndManager.h"

int GetRandomNumber(int min, int max);
int GetRandomNumber(int min, int max, int ignoreNumber);

Position AbsolutePosition(Position pos1);

bool ProbabilityCheck(int probability);

vector<Position> GetAllNeighborOffset(Position squareLimit);

//* by default it ignores zero
Position GetRandomPositionCustom(Position xLimit, Position yLimit, bool ignoreZero = true);
Position GetRandomPositionCustom(Position limit,  bool ignoreZero = true);
Position GetRandomPosition(Position limit);
Position GetRandomPositionInBounds(Position currentPosition, int range);

bool IsPositionValidAndUnoccupied(const Position& pos);
bool IsPositionValid(const Position& pos);

float GetDistance(const Position& pos1, const Position& pos2);
Position GetClosestPosition(const Position currentPos, const vector<Position>& positions);

int GetShootDistance(Position currentPosition, Position enemyPosition);

Position GetNeighborPosition(Position pos, int direction);
vector<Position> GetAllNeighborOffsetExtension(Position pos);


