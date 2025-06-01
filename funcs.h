/*
file: funcs.h
Course: Object Oriented Programming & Data Structures
Trimester: 2510
+===========================+============+=============================================+================+
|           NAME            |     ID     |                    EMAIL                    | Phone Number   |
+===========================+============+=============================================+================+
| Mohammad Muazzam Ali      | 242UC244PZ |   mohammad.muazzam.ali@student.mmu.edu.my   |  011-5630 3174 |
| Bin Mohammad Shoaib Ali   |            |                                             |                |
+---------------------------+------------+---------------------------------------------+----------------+
| Muhammad Aish Qayyim bin  | 242UC244BX |   muhammad.aish.qayyim@student.mmu.edu.my   |  012-342 5662  |
| Mohd Azmi                 |            |                                             |                |
+---------------------------+------------+---------------------------------------------+----------------+
| Muhammad Aref bin Mohd    | 242UC244Q1 |    muhammad.aref.mohd@student.mmu.edu.my    |  010-650 9597  |
| Hasni                     |            |                                             |                |
+---------------------------+------------+---------------------------------------------+----------------+
| Muhammad Fahmi bin Hanafi | 242UC244FX |   muhammad.fahmi.hanafi@student.mmu.edu.my  + 011-3174 3545  |
+===========================+============+=============================================+================+
*/

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


