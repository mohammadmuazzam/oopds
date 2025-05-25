#include "funcs.h"

bool ProbabilityCheck(int probability)
{
    int randomValue = rand() % 100;
    return randomValue <= probability;
}

int GetRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

Position GetRandomPosition(Position limit) {
    Position pos;
    pos.x = GetRandomNumber(0, limit.x);
    pos.y = GetRandomNumber(0, limit.y);
    return pos;
}

Position GetRandomPositionCustom(Position xLimit, Position yLimit, bool ignoreZero) {
    Position pos;
    if (ignoreZero)
    {
        while (pos.x == 0)
        {
            pos.x = GetRandomNumber(xLimit.x, xLimit.y);
        }
        while (pos.y == 0)
        {
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

bool IsPositionValid(const Position& pos, const Position& mapSize) {
    for (const auto& robot : simulationManager.robots)
    {
        //* check if the position is occupied by another robot
        if (robot->getPosition().x == pos.x && robot->getPosition().y == pos.y)
        {
            return false;
        }
    }
    return (pos.x >= 0 && pos.x < mapSize.x && pos.y >= 0 && pos.y < mapSize.y);
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