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

void GenericRobot::move(int direction)
{
    Position finalPosition = position;
    finalPosition = GetNeighborPosition(finalPosition, direction);

    cout << getType() << "-" << name << " moving to (" << finalPosition.x << ", " << finalPosition.y << ")" << endl;

    //* check if out of bounds
    if (0 <= finalPosition.x && finalPosition.x < simulationManager.mapSize.x)
    {
        position.x = finalPosition.x;
    }
    if (0 <= finalPosition.y && finalPosition.y < simulationManager.mapSize.y)
    {
        position.y = finalPosition.y;
    }
}

void GenericRobot::look(int direction)
{
    Position lookPosition = GetNeighborPosition(position, direction);

    cout << getType() << "-" << name << " looking at (" << lookPosition.x << ", " << lookPosition.y << ")" << endl;

    if (simulationManager.isPositionOccupied(lookPosition))
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
    Position spawnPosition = GetRandomPosition(simulationManager.mapSize);
    while (simulationManager.isPositionOccupied(spawnPosition))
    {
        spawnPosition = GetRandomPosition(simulationManager.mapSize);
    }
    setPosition(spawnPosition);
}

void GenericRobot::shoot(Position enemyPosition)
{
    
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
    return "GenericRobot";
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

void MovingRobot::move(int direction)
{
    Position finalPosition = position;
    finalPosition = GetNeighborPosition(finalPosition, direction);

    cout << getType() << "-" << name << " moving to (" << finalPosition.x << ", " << finalPosition.y << ")" << endl;

    //* check if out of bounds
    if (0 <= finalPosition.x && finalPosition.x < simulationManager.mapSize.x)
    {
        position.x = finalPosition.x;
    }
    if (0 <= finalPosition.y && finalPosition.y < simulationManager.mapSize.y)
    {
        position.y = finalPosition.y;
    }
}

#pragma region MovingRobot


string MovingRobot::getType()
{
    return "MovingRobot";
}

SimulationManager simulationManager;