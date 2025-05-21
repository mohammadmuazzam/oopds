#include "robots.h"
#include "simulationData.h"

void GenericRobot::move(int direction)
{
    switch (direction)
    {
        case 0: // top left
            position.x--;
            position.y++;
            break;
        case 1: // top
            position.y++;
            break;
        case 2: // top right
            position.x++;
            position.y++;
            break;
        case 3: // left
            position.x--;
            break;
        case 4: // right
            position.x++;
            break;
        case 5: // bottom left
            position.x--;
            position.y--;
            break;
        case 6: // bottom
            position.y--;
            break;
        case 7: // bottom right
            position.x++;
            position.y--;
            break;
    }
    //* check if out of bounds
    if (position.x < 0)
        position.x = 0;
    if (position.y < 0)
        position.y = 0;
    
    if (position.x > mapSize.x)
        position.x = mapSize.x;
    if (position.y > mapSize.y)
        position.y = mapSize.y;
}