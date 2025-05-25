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

    //* check if out of bounds
    if (0 <= newPosition.x && newPosition.x < simulationManager.mapSize.x)
    {
        position.x = newPosition.x;
    }
    if (0 <= newPosition.y && newPosition.y < simulationManager.mapSize.y)
    {
        position.y = newPosition.y;
    }
}

void GenericRobot::look(Position lookPosition)
{
    cout << getType() << "-" << name << " looking at (" << lookPosition.x << ", " << lookPosition.y << ")" << endl;

    if (simulationManager.isPositionOccupied(position + lookPosition))
    {
        enemyPosition = lookPosition;
    }
    else
    {
        enemyPosition = Position(-1, -1);
    }
}

void GenericRobot::die()
{
    cout << getType() << "-" << name << " died" << endl;
    health--;
    upgrades.clear();
    numBullets = 10;

    //* remove robot from simulation manager
    if (health <= 0)
    {
        simulationManager.robots.erase(remove_if(simulationManager.robots.begin(), simulationManager.robots.end(),
            [&](const unique_ptr<GenericRobot>& robot) { return robot.get() == this; }), simulationManager.robots.end());
    }
}

void GenericRobot::spawn()
{
    //* spawn robot at a random position
    Position spawnPosition = GetRandomPosition(simulationManager.mapSize - 1);
    while (simulationManager.isPositionOccupied(spawnPosition))
    {
        spawnPosition = GetRandomPosition(simulationManager.mapSize - 1);
    }
    setPosition(spawnPosition);
}

void GenericRobot::shoot(Position enemyPosition)
{
    //! if enemyPosition is (-1, -1), it means that the robot did not see any enemy
    //! so robot will shoot at a random position
    
    //* if robot saw enemy
    if (enemyPosition != Position(-1, -1))
    {
        cout << getType() << "-" << name << " shooting at (" << enemyPosition.x << ", " << enemyPosition.y << ")" << endl;
        //* check if enemy exists at the position
        if (simulationManager.getRobotAtPosition(enemyPosition) != nullptr)
        {
            GenericRobot* enemyRobot = simulationManager.getRobotAtPosition(enemyPosition);
            if (ProbabilityCheck(70))
            {
                cout << getType() << "-" << name << " hit enemy robot " << enemyRobot->name << endl;
                enemyRobot->die();

                if (enemyRobot->health > 0)
                {
                    cout << "spawning enemy robot " << enemyRobot->name << endl;
                    enemyRobot->spawn();
                }
                    
            }
            return;
        }
    }
    else
    {
        cout << getType() << "-" << name << " shooting at empty space (" << enemyPosition.x << ", " << enemyPosition.y << ")" << endl;
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
}

GenericRobot::~GenericRobot()
{
}

#pragma endregion

#pragma region MovingRobot

MovingRobot::MovingRobot()
{
    numBullets = 10;
    type = "MovingRobot";
}

#pragma endregion 



SimulationManager simulationManager;