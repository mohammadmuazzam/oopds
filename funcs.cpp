#include "funcs.h"
#include <cmath>
#include <iostream>

using namespace std;

float GetDistance(const Position& pos1, const Position& pos2)
{
    return sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));
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
    return distance;
}

Position GetClosestPosition(const vector<Position>& positions)
{
    if (positions.empty())
        return Position(-1, -1);

    Position closest = positions[0];
    float minDistance = std::numeric_limits<float>::max();

    //* linear search for the closest position
    for (const auto& pos : positions)
    {
        float distance = GetDistance(closest, pos);
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