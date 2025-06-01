/*
file: funcs.cpp
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

#include "funcs.h"
#include <cmath>
#include <iostream>

using namespace std;

float GetDistance(const Position& pos1, const Position& pos2)
{
    float a = pos1.x - pos2.x;
    float b = pos1.y - pos2.y;
    //* calculate the Euclidean distance
    float result = sqrt(a * a + b * b);
    return result;
}

Position AbsolutePosition(Position pos1)
{
    Position result;
    result.x = abs(pos1.x);
    result.y = abs(pos1.y);
    return result;
}

//! Get the distance for shooting (Manhattan distance)
int GetShootDistance(Position currentPosition, Position enemyPosition)
{
    Position absDistanceVector = AbsolutePosition(currentPosition - enemyPosition);
    int distance = absDistanceVector.x + absDistanceVector.y;
    return distance-1;
}

Position GetClosestPosition(const Position currentPos, const vector<Position>& positions)
{
    if (positions.empty())
        return Position(-1, -1);

    Position closest = positions[0];
    float minDistance = std::numeric_limits<float>::max();

    //* linear search for the closest position
    for (const auto& pos : positions)
    {
        float distance = GetDistance(currentPos, pos);
        if (distance < minDistance)
        {
            minDistance = distance;
            closest = pos;
        }
    }

    return closest;
}

bool ProbabilityCheck(int probability)
{
    int randomValue = rand() % 100;
    return randomValue <= probability;
}

int GetRandomNumber(int min, int max) {
    if (min == max)
        return min;
    if (min > max) 
        swap(min, max);
    return rand() % (max - min + 1) + min;
}

int GetRandomNumber(int min, int max, int ignoreNumber) {
    int randomValue;
    do {
        randomValue = GetRandomNumber(min, max);
    } while (randomValue == ignoreNumber);
    return randomValue;
}

Position GetRandomPosition(Position limit) {
    Position pos;
    pos.x = GetRandomNumber(1, limit.x);
    pos.y = GetRandomNumber(1, limit.y);
    return pos;
}

Position GetRandomPositionCustom(Position xLimit, Position yLimit, bool ignoreZero) {
    Position pos;
    if (ignoreZero)
    {
        while (pos.x == 0 && pos.y == 0)
        {
            pos.x = GetRandomNumber(xLimit.x, xLimit.y);
            pos.y = GetRandomNumber(yLimit.x, yLimit.y);
        }
    }
    else
    {
        pos.x = GetRandomNumber(xLimit.x, xLimit.y);
        pos.y = GetRandomNumber(yLimit.x, yLimit.y);
    }
    
    return pos;
}

Position GetRandomPositionCustom(Position limit, bool ignoreZero) {
    Position pos;
    if (ignoreZero)
    {
        while (pos.x == 0 && pos.y == 0)
        {
            pos.x = GetRandomNumber(limit.x, limit.y);
            pos.y = GetRandomNumber(limit.x, limit.y);
        }
    }
    else
    {
        pos.x = GetRandomNumber(0, limit.x);
        pos.y = GetRandomNumber(0, limit.y);
    }
    
    return pos;
}

Position GetRandomPositionInBounds(Position currentPosition, int range) {
    Position pos = Position(0, 0);
    while (pos.x == 0 && pos.y == 0)
    {
        pos.x = (currentPosition.x == 1) ? GetRandomNumber(0, range) : 
            (currentPosition.x == simulationManager.mapSize.x) ? -GetRandomNumber(0, range) : GetRandomNumber(-range, range);
        pos.y = (currentPosition.y == 1) ? GetRandomNumber(0, range) :
            (currentPosition.y == simulationManager.mapSize.y) ? -GetRandomNumber(0, range) : GetRandomNumber(-range, range);

    }
    return pos;
}

bool IsPositionValidAndUnoccupied(const Position& pos) {
    for (const auto& robot : simulationManager.robots)
    {
        //* check if the position is occupied by another robot
        if (robot->getPosition().x == pos.x && robot->getPosition().y == pos.y)
        {
            return false;
        }
    }
    bool isValidX = (1 <= pos.x && pos.x <= simulationManager.mapSize.x);
    bool isValidY = (1 <= pos.y && pos.y <= simulationManager.mapSize.y);
     
    return (isValidX && isValidY);
}

bool IsPositionValid(const Position& pos) {
    bool isValidX = (1 <= pos.x && pos.x <= simulationManager.mapSize.x);
    bool isValidY = (1 <= pos.y && pos.y <= simulationManager.mapSize.y);
    
    return (isValidX && isValidY);
}

vector<Position> GetAllNeighborOffset(Position squareLimit)
{
    vector<Position> offsets;
    for (int i = squareLimit.x; i <= squareLimit.y; i++)
    {
        for (int j = squareLimit.x; j <= squareLimit.y; j++)
        {
            if (i == 0 && j == 0) continue; // skip the center position
            offsets.push_back(Position(i, j));
        }
    }
    return offsets;
}

Position GetNeighborPosition(Position pos, int direction)
{
    Position finalPosition = pos;
    switch (direction)
    {
        case 0: // top left
            finalPosition.x--;
            finalPosition.y++;
            break;
        case 1: // top
            finalPosition.y++;
            break;
        case 2: // top right
            finalPosition.x++;
            finalPosition.y++;
            break;
        case 3: // left
            finalPosition.x--;
            break;
        case 4: // right
            finalPosition.x++;
            break;
        case 5: // bottom left
            finalPosition.x--;
            finalPosition.y--;
            break;
        case 6: // bottom
            finalPosition.y--;
            break;
        case 7: // bottom right
            finalPosition.x++;
            finalPosition.y--;
            break;
    }
    return finalPosition;
}

vector<Position> GetAllNeighborOffsetExtension(Position pos)
{
    vector<Position> offsets;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0) continue; // skip the center position
            offsets.push_back(Position(pos.x + i, pos.y + j));
        }
    }
    return offsets;
}