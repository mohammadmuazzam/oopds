#include <string>
#include <vector>
#include "robotsAndManager.h"
#include "funcs.h"
#include <algorithm>
#include <iostream>

using namespace std;

#pragma region SimulationManager

bool SimulationManager::isPositionOccupied(Position pos)
{
    for (const auto& robot : robots)
    {
        if (robot->getPosition() == pos)
        {
            return true;
        }
    }

    return false;
}

GenericRobot* SimulationManager::getRobotAtPosition(Position pos)
{
    for (const auto& robot : robots)
    {
        if (robot->getPosition() == pos)
        {
            return robot.get();
        }
    }

    return nullptr;
}
#pragma endregion
#pragma region GenericRobot

bool GenericRobot::isDead()
{
    return health <= 0;
}

void GenericRobot::setPosition(Position newPosition)
{
    position = newPosition;
}

Position GenericRobot::getPosition() const  
{
    return position;
}

void GenericRobot::move(Position movePosition)
{
    Position newPosition = position + movePosition;

    if (simulationManager.isPositionOccupied(newPosition))
    {
        cout << "\t(" << newPosition.x << ", " << newPosition.y << ") is occupied. Cannot move there." << endl;
        return;
    }
    
    //* check if out of bounds
    if (0 <= newPosition.x && newPosition.x < simulationManager.mapSize.x)
    {
        position.x = newPosition.x;
    }
    if (0 <= newPosition.y && newPosition.y < simulationManager.mapSize.y)
    {
        position.y = newPosition.y;
    }



    cout << "moved to (" << position.x << ", " << position.y << ")" << endl;

}

void GenericRobot::look(Position lookPosition)
{
    cout << "looked at (" << lookPosition.x << ", " << lookPosition.y << ")" << endl;
    if (simulationManager.isPositionOccupied(position + lookPosition))
    {
        enemyPosition = lookPosition;
    }
    else
    {
        enemyPosition = Position(-1, -1);
    }
}

void GenericRobot::spawn()
{
    //* spawn robot at a random position
    Position spawnPosition = GetRandomPosition(simulationManager.mapSize);
    while (simulationManager.isPositionOccupied(spawnPosition))
    {
        spawnPosition = GetRandomPosition(simulationManager.mapSize);
    }
    setPosition(spawnPosition);
}

void GenericRobot::die()
{
    health--;
    upgrades.clear();
    numBullets = 10;

    cout << "\t## Robot " << name << " died (health = " << health << ") ## " << endl;

    //* remove robot from simulation manager
    if (health <= 0)
    {
        cout << "\tRobot " << name << " has died and will be removed from the simulation." << endl;
        simulationManager.robots.erase(remove_if(simulationManager.robots.begin(), simulationManager.robots.end(),
            [&](const unique_ptr<GenericRobot>& robot) { return robot.get() == this; }), simulationManager.robots.end());
        
        cout << "\tRemaining robots:" << endl;
        for (const auto& robot : simulationManager.robots)
        {
            cout << "\t\t" << robot->name << endl;
        }
    }
    else
    {
        spawn();
        cout << "\t\t Respawning at (" << position.x << ", " << position.y << ")" << endl;
    }
}

void GenericRobot::shoot(Position enemyPosition)
{
    //* shoot at shoot position
    Position shootPosition;
    
    if (enemyPosition != Position(-1, -1))
        shootPosition = enemyPosition;
    else
        shootPosition = GetRandomPositionCustom(Position(-1, 1), Position(-1, 1)) + position;
    
    
    if (simulationManager.getRobotAtPosition(shootPosition) != nullptr)
    {
        GenericRobot* enemyRobot = simulationManager.getRobotAtPosition(shootPosition);
        cout << "TARGET ACQUIRED: " << enemyRobot->name << endl;
        if (ProbabilityCheck(70))
        {
            cout << "HIT " << enemyRobot->getType() << ", " << enemyRobot->name  << endl;
            enemyRobot->die();
        }
        else
        {
            cout << "MISS " << enemyRobot->getType() << ", " << enemyRobot->name << endl;
        }
    }
    else
    {
        cout << "NO ROBOT (" << shootPosition.x << ", " << shootPosition.y << ")" << endl;
    }

    numBullets--;

    // check if out of bullets
    if (numBullets <= 0)
    {
        cout << getType() << "-" << name << " ran out of bullets" << endl;
        die();
    }
}

string GenericRobot::getType()
{
    return type;
}

GenericRobot::GenericRobot()
{
    numBullets = 10;
    health = 3;
    type = "GenericRobot";
}

GenericRobot::~GenericRobot()
{
}

#pragma endregion

#pragma region MovingRobot

MovingRobot::MovingRobot()
{
    type = "MovingRobot";
}

#pragma endregion 



SimulationManager simulationManager;