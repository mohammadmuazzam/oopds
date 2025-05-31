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

void GenericRobot::move(Position moveDistance)
{
    Position newPosition = position + moveDistance;

    if (!IsPositionValidAndUnoccupied(newPosition))
    {
        cout << "\t(" << newPosition.x << ", " << newPosition.y << ") is occupied or out of bounds. Cannot move there." << endl;
        return;
    }

    position = newPosition;
    cout << "moved to (" << position.x << ", " << position.y << ")" << endl;    
}

void GenericRobot::look(Position lookPosition)
{
    cout << "looked at (" << lookPosition.x + position.x << ", " << lookPosition.y + position.y << ")" << endl;
    if (IsPositionValidAndUnoccupied(position + lookPosition))
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
    while (!IsPositionValidAndUnoccupied(spawnPosition))
    {
        cout << "\t\tPosition (" << spawnPosition.x << ", " << spawnPosition.y << ") is occupied or out of bounds. Respawning." << endl;
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
        simulationManager.deadRobots.push_back(name);
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
        if (ProbabilityCheck(70) && enemyRobot->isVisible)
        {
            cout << "HIT " << enemyRobot->getType() << ", " << enemyRobot->name  << endl;
            enemyRobot->die();
            //* upgrade robot
        }
        else
        {
            cout << "MISS " << enemyRobot->getType() << ", " << enemyRobot->name << endl;
        }

        enemyPosition = Position(-1, -1); // reset enemy position after shooting
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
    int upgradeTypeInt;
    if (upgrades.size() >= 3)
    {
        cout << "Maximum upgrades reached. No more upgrades can be applied." << endl;
        return;
    }
    //! CONTINUE HERE
    if (upgrades.size() == 1)
    {
        for (const auto& upgrade : upgrades)
        {
            upgradeTypeInt = static_cast<int>(upgrade->getUpgradeType());
        }
    }

    int upgradeArea = GetRandomNumber(0, 2);

    switch (upgradeArea)
    {
        case 0: //* upgrade moving
            //TODO: implement moving upgrade, choose randomly between hidebot or jumpbot
            break;
        case 1: //* upgrade looking
{
        int choice = GetRandomNumber(0, 1);
        if (choice == 0) {
            upgrades.emplace_back(make_unique<ScoutRobot>(this));
            cout << name << " upgraded to ScoutBot." << endl;
        } else {
            upgrades.emplace_back(make_unique<TrackRobot>(this));
            cout << name << " upgraded to TrackBot." << endl;
        }
    break;
}
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

#pragma region ThinkingRobot
ThinkingRobot::ThinkingRobot()
{
    type = "ThinkingRobot";
    lastEnemyPositions.push_back(enemyPosition);
}

void ThinkingRobot::think()
{
    if (lookLimit == 0)
    {
        lookLimit = GetRandomNumber(2, 5);
        return;
    }

    //* if no data about enemy positions, then look around
    if (lastEnemyPositions.empty() && lookCount <= lookLimit)
    {
        cout << "\t... No known enemy positions. Looking around." << endl;
        look(GetRandomPositionCustom(Position(-lookRange, lookRange)));
        if (lastEnemyPositions.empty())
        {
            cout << "\t... No enemy found in sight." << endl;
        }
        else
        {
            cout << "\t... Found enemy positions: ";
            for (const auto& pos : lastEnemyPositions)
            {
                cout << "(" << pos.x << ", " << pos.y << ") ";
            }
            cout << endl;
        }
        return;
    }

    //* if lookCount exceeds a random number and still no data about enemy, then move randomly
    if (lookCount > lookLimit && lastEnemyPositions.empty())
    {
        lookCount = 0; // reset look count
        lookLimit = 0; // reset look limit1
        cout << "\tLook count exceeded and no enemy in sight. Moving randomly.\n\t" << endl;
        move(GetRandomPositionCustom(Position(-moveSteps, moveSteps)));
        return;
    }

    //* if last enemy position isn't in shooting range, then move towards closest known enemy position
    if (!lastEnemyPositions.empty())
    {
        Position closestEnemyPosition = GetClosestPosition(lastEnemyPositions);
        if (GetShootDistance(position, closestEnemyPosition) > shootRange)
        {
            Position movePosition = closestEnemyPosition - position;
            if (movePosition.x > moveSteps) movePosition.x = moveSteps;
            if (movePosition.y > moveSteps) movePosition.y = moveSteps;

            move(movePosition);
            return;
        }
        else
        {
            cout << "SHOOT" << endl;
            shoot(closestEnemyPosition);
            //* remove the closest enemy position from the list
            lastEnemyPositions.erase(remove(lastEnemyPositions.begin(), lastEnemyPositions.end(), closestEnemyPosition), lastEnemyPositions.end());
            return;
        }
    }



    //* if saw enemy, then shoot at that position
    

    //* 
}

void ThinkingRobot::look(Position lookPosition)
{
    cout << "looked at (" << lookPosition.x << ", " << lookPosition.y << ")" << endl;
    lookCount++;

    //* if scoutbot ability is available, then look at whole map
    for (const auto &upgrade : upgrades)
    {
        if (upgrade->getUpgradeName() == UpgradeName::ScoutBot)
        {
            cout << "ScoutBot ability activated. Looking at the entire map." << endl;
            upgrade->upgradedAbility();
            
            ScoutRobot* scout = dynamic_cast<ScoutRobot*>(upgrade.get());
            if (scout && !scout->abilityUsed)
            {
                lastEnemyPositions = scout->enemyPositions;
            }
            else
            {
                cout << "Error: ScoutRobot upgrade is not of type ScoutRobot." << endl;
            }
            return;
        }
    }

    GenericRobot::look(lookPosition);
    
    if (enemyPosition != Position(-1, -1))
    {
        lastEnemyPositions.push_back(enemyPosition);
    }
}
#pragma endregion


SimulationManager simulationManager;

#pragma region Shooting Upgrades

void LongShotBot::upgradedAbility()
{
    cout << "LongShotBot ability activated: Can shoot targets where x + y <= 3." << endl;

    for (auto& robot : simulationManager.robots)
    {
        if (find_if(robot->upgrades.begin(), robot->upgrades.end(), [](const unique_ptr<UpgradeRobot>& upgrade) {
            return upgrade->getUpgradeName() == UpgradeName::LongShotBot;
        }) != robot->upgrades.end())
        {
            robot->shootRange = 3;
        }
    }
}

void SemiAutoBot::upgradedAbility()
{
    cout << "SemiAutoBot ability activated: Fires 3 rapid shells at one location, each with 70% chance to hit." << endl;

    for (auto& robot : simulationManager.robots)
    {
        if (find_if(robot->upgrades.begin(), robot->upgrades.end(), [](const unique_ptr<UpgradeRobot>& upgrade) {
            return upgrade->getUpgradeName() == UpgradeName::SemiAutoBot;
        }) != robot->upgrades.end())
        {
            Position target = robot->enemyPosition;

            if (target != Position(-1, -1))
            {
                GenericRobot* targetRobot = simulationManager.getRobotAtPosition(target + robot->getPosition());
                if (targetRobot != nullptr)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        if (ProbabilityCheck(70))
                        {
                            cout << "SEMI-AUTO HIT " << targetRobot->getType() << ", " << targetRobot->name << endl;
                            targetRobot->die();

                            if (targetRobot->isDead()) break;
                        }
                        else
                        {
                            cout << "SEMI-AUTO MISS " << targetRobot->getType() << ", " << targetRobot->name << endl;
                        }
                    }
                }
                else
                {
                    cout << "SEMI-AUTO NO TARGET at (" << target.x + robot->getPosition().x << ", " << target.y + robot->getPosition().y << ")" << endl;
                }

                robot->setNumBullets(robot->getNumBullets() - 1);
                if (robot->getNumBullets() <= 0)
                {
                    cout << robot->getType() << "-" << robot->name << " ran out of bullets" << endl;
                    robot->die();
                }
            }
        }
    }
}

void ThirtyShellBot::upgradedAbility()
{
    cout << "ThirtyShellBot ability activated: Ammo reloaded to 30 shells." << endl;

    for (auto& robot : simulationManager.robots)
    {
        if (find_if(robot->upgrades.begin(), robot->upgrades.end(), [](const unique_ptr<UpgradeRobot>& upgrade) {
            return upgrade->getUpgradeName() == UpgradeName::ThirtyShellBot;
        }) != robot->upgrades.end())
        {
            robot->setNumBullets(30);
        }
    }
}

void ScoutRobot::upgradedAbility() {
    if (abilityUsed) {
        cout << "Ability already used." << endl;
        return;
    }

    cout << "Scanning entire battlefield..." << endl;

    enemyPositions.clear();

    //Loop through all robots in the simulation
    for (const auto& r : simulationManager.robots) {
        if (r.get() != robot && !r->isDead()) {
            Position enemyPos = r->getPosition();
            enemyPositions.push_back(enemyPos);
            cout << "\tEnemy: " << r->name << " at (" << enemyPos.x << ", " << enemyPos.y << ")" << endl;
        }
    }

    abilityUsed = true;
}

void TrackRobot::upgradedAbility() {
    if (trackersRemaining <= 0) {
        cout << "No more trackers available." << endl;
        return;
    }

    cout << "[TrackBot] Scanning nearby for enemies..." << endl;

    //Allow the robot to check surrounding
    vector<Position> offsets = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (const auto& offset : offsets) {
        Position checkPos = robot->getPosition() + offset;
        GenericRobot* target = simulationManager.getRobotAtPosition(checkPos);

        if (target && target != robot && !target->isDead()) {
            if (std::find(trackedRobots.begin(), trackedRobots.end(), target) == trackedRobots.end()) {
                trackedRobots.push_back(target);
                trackersRemaining--;
                cout << "[TrackBot] Tracker placed on: " << target->name << endl;
                return;
            }
        }
    }

    cout << "[TrackBot] No valid nearby enemy to track." << endl;
}

//Return the live position of all currently tracked enemy robots
vector<Position> TrackRobot::getTrackedPositions() const {
    vector<Position> positions;
    for (const auto& tracked : trackedRobots) {
        if (!tracked->isDead()) {
            positions.push_back(tracked->getPosition());
        }
    }
    return positions;
}


#pragma endregion
