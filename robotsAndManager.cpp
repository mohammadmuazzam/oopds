#include <string>
#include <vector>
#include "robotsAndManager.h"
#include "funcs.h"
#include <algorithm>
#include <iostream>

using namespace std;

#pragma region SimulationManager



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

string GenericRobot::getType() const
{
    return type;
}

int GenericRobot::getMoveSteps() const
{
    return moveSteps;
}

int GenericRobot::getLookRange() const
{
    return lookRange;
}

void GenericRobot::move(Position movePosition)
{
    Position newPosition = position + movePosition;

    if (IsPositionValidOrOccupied(newPosition))
    {
        cout << "\t(" << newPosition.x << ", " << newPosition.y << ") is occupied or out of bounds. Cannot move there." << endl;
        return;
    }



    cout << "moved to (" << position.x << ", " << position.y << ")" << endl;

}

void GenericRobot::look(Position lookPosition)
{
    cout << "looked at (" << lookPosition.x << ", " << lookPosition.y << ")" << endl;
    if (IsPositionValidOrOccupied(position + lookPosition))
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
    while (IsPositionValidOrOccupied(spawnPosition))
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
            //* upgrade robot
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

void GenericRobot::upgradeRandom()
{
    int upgradeArea = GetRandomNumber(0, 2);

    switch (upgradeArea)
    {
        case 0: //* upgrade moving
            //TODO: implement moving upgrade, choose randomly between hidebot or jumpbot
            break;
        case 1: //* upgrade looking
            //TODO: implement looking upgrade, choose randomly between scoutbot or trackbot
            break;
        case 2: //* upgrade shooting
            //TODO: implement shooting upgrade, choose randomly between longshotbot or semiautobot or thirtyshellbot
            break;
        default:
            cout << "No upgrade applied." << endl;
    }
}


GenericRobot::~GenericRobot()
{
}

#pragma endregion

#pragma region MovingRobot

MovingRobot::MovingRobot()
{
    type = "MovingRobot";
    moveSteps = 2; 
}

#pragma endregion 

#pragma region ShootingRobot

ShootingRobot::ShootingRobot()
{
    type = "ShootingRobot";
    numBullets = 15; 
};

#pragma endregion

#pragma region LookingRobot

LookingRobot::LookingRobot()
{
    type = "LookingRobot";
    lookRange = 2; 
}

#pragma endregion


SimulationManager simulationManager;