#include "funcs.h"

bool ProbabilityCheck(int probability)
{
    int randomValue = rand() % 100;
    return randomValue <= probability;
}

int GetRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

Position GetRandomPosition(Position mapSize) {
    Position pos;
    pos.x = GetRandomNumber(0, mapSize.x - 1);
    pos.y = GetRandomNumber(0, mapSize.y - 1);
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