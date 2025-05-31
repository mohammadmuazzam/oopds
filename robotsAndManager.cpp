#include <string>
#include <vector>
#include "robotsAndManager.h"
#include "funcs.h"
#include <algorithm>
#include <iostream>

using namespace std;

#pragma region ostream
ostream& operator<<(ostream& os, const Position& pos)
{
    os << "(" << pos.x << ", " << pos.y << ")";
    return os;
}

ostream& operator<<(ostream& os, const RobotType& type)
{
    switch (type)
    {
        case RobotType::GenericRobot: os << "GenericRobot"; break;
        case RobotType::MovingRobot: os << "MovingRobot"; break;
        case RobotType::ShootingRobot: os << "ShootingRobot"; break;
        case RobotType::LookingRobot: os << "LookingRobot"; break;
        case RobotType::ThinkingRobot: os << "ThinkingRobot"; break;
    }
    return os;
}

ostream& operator<<(ostream& os, const UpgradeType& type)
{
    switch (type)
    {
        case UpgradeType::MovingUpgrade: os << "MovingUpgrade"; break;
        case UpgradeType::ShootingUpgrade: os << "ShootingUpgrade"; break;
        case UpgradeType::LookingUpgrade: os << "LookingUpgrade"; break;
    }
    return os;
}

ostream& operator<<(ostream& os, const UpgradeName& name)
{
    switch (name)
    {
        case UpgradeName::ScoutBot: os << "ScoutBot"; break;
        case UpgradeName::TrackingBot: os << "TrackingBot"; break;
        case UpgradeName::SemiAutoBot: os << "SemiAutoBot"; break;
        case UpgradeName::ThirtyShellBot: os << "ThirtyShellBot"; break;
        case UpgradeName::LongShotBot: os << "LongShotBot"; break;
        case UpgradeName::HideBot: os << "HideBot"; break;
        case UpgradeName::JumpBot: os << "JumpBot"; break;
    }
    return os;
}

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
        cout << "\t\tRespawning at (" << position.x << ", " << position.y << ")" << endl;
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
            cout << "\tHIT " << enemyRobot->getType() << ", " << enemyRobot->name  << endl;
            enemyRobot->die();
            //* upgrade robot
            upgradeRandom();
        }
        else
        {
            cout << "\tMISS " << enemyRobot->getType() << ", " << enemyRobot->name << endl;
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
    int upgradeArea;

    vector<UpgradeType> availableUpgrades = { UpgradeType::MovingUpgrade, UpgradeType::ShootingUpgrade, UpgradeType::LookingUpgrade };
    vector<UpgradeType> appliedUpgrades;
    //* return if maximum upgrades reached
    if (upgrades.size() >= 3)
    {
        cout << "Maximum upgrades reached. No more upgrades can be applied." << endl;
        return;
    }

    for (const auto& upgrade : upgrades)
    {
        cout << "\tAlready applied upgrade: " << upgrade->getUpgradeName() << "(" << upgrade->getUpgradeType() << ")" << endl;
        appliedUpgrades.push_back(upgrade->getUpgradeType());
        availableUpgrades.erase(remove(availableUpgrades.begin(), 
                                availableUpgrades.end(), 
                                upgrade->getUpgradeType()), 
                                availableUpgrades.end());
    }
    
    //* for slightly faster simulation
    if (upgrades.size() == 1) //* get random upgrade ignoring the applied upgrade
        upgradeArea = (GetRandomNumber(0, 1) == 0) ? static_cast<int>(availableUpgrades[0]) : static_cast<int>(availableUpgrades[1]);
    else if (upgrades.size() == 2)
        upgradeArea = static_cast<int>(availableUpgrades[0]); //* only one upgrade left to apply
    else //* if no upgrade yet
        upgradeArea = GetRandomNumber(0, 2);

    int randomUpgradeName;

    switch (upgradeArea)
    {
        case 0: //* upgrade moving
            randomUpgradeName = GetRandomNumber(0, 1); // 0 = HideBot, 1 = JumpBot
            if (randomUpgradeName == 0)
            {
                cout << "\tApplying HideBot upgrade." << endl;
                upgrades.push_back(make_unique<HideBot>(this));
            }
            else
            {
                cout << "\tApplying JumpBot upgrade." << endl;
                upgrades.push_back(make_unique<JumpBot>(this));
            }
            break;
        case 1: //* upgrade looking
            {
                randomUpgradeName = GetRandomNumber(0, 1);
                if (randomUpgradeName == 0) 
                {
                    upgrades.push_back(make_unique<ScoutRobot>(this));
                    cout << "\tApplying ScoutBot upgrade" << endl;
                } else 
                {
                    upgrades.push_back(make_unique<TrackRobot>(this));
                    cout << "\tApplying TrackBot upgrade" << endl;
                }
                break;
            }
            break;
        case 2: //* upgrade shooting
            randomUpgradeName = GetRandomNumber(0, 2); // 0 = SemiAutoBot, 1 = ThirtyShellBot, 2 = LongShotBot
            switch (randomUpgradeName)
            {
                case 0:
                    cout << "\tApplying SemiAutoBot upgrade." << endl;
                    upgrades.push_back(make_unique<SemiAutoBot>(this));
                    break;
                case 1:
                    cout << "\tApplying ThirtyShellBot upgrade." << endl;
                    upgrades.push_back(make_unique<ThirtyShellBot>(this));
                    break;
                case 2:
                    cout << "\tApplying LongShotBot upgrade." << endl;
                    upgrades.push_back(make_unique<LongShotBot>(this));
                    break;
            }
            break;
        default:
            cout << "Error: No upgrade applied. upgradeArea: " << upgradeArea << endl;
    }
}


GenericRobot::~GenericRobot()
{
}

#pragma endregion

#pragma region MovingRobot

MovingRobot::MovingRobot()
{
    type = RobotType::MovingRobot;
    moveSteps = 2; 
}

#pragma endregion 

#pragma region ShootingRobot

ShootingRobot::ShootingRobot()
{
    type = RobotType::ShootingRobot;
    numBullets = 15; 
};

#pragma endregion

#pragma region LookingRobot

LookingRobot::LookingRobot()
{
    type = RobotType::LookingRobot;
    lookRange = 2; 
}

#pragma endregion

#pragma region ThinkingRobot
ThinkingRobot::ThinkingRobot()
{
    type = RobotType::ThinkingRobot;
}

void ThinkingRobot::think()
{
    if (lookLimit == 0)
    {
        lookLimit = GetRandomNumber(1, 3);
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
        cout << "\t... Moving randomly.\n\t";
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
            cout << "\t... Moving towards last known enemy position\n\t";
            move(movePosition);
            return;
        }
        else
        {
            cout << "\t... In range with last known enemy position. Shooting." << endl;
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
    cout << "\t... ";
    GenericRobot::look(lookPosition);
    
    if (enemyPosition != Position(-1, -1))
    {
        cout << "\t...Enemy found at (" << enemyPosition.x + position.x << ", " << enemyPosition.y + position.y << ")" << endl;
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
#pragma endregion

#pragma region Looking Upgrades
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

#pragma region Moving Upgrades
// HideBot ability implementation
void HideBot::upgradedAbility()
{
    if (hideCount > 0)
    {
        cout << "HideBot ability activated: Robot is now hidden and cannot be hit." << endl;
        robot->setIsVisible(false);
        hideCount--;
    }
    else
    {
        cout << "HideBot: No more hides left." << endl;
    }
}

// JumpBot ability implementation
void JumpBot::upgradedAbility()
{
    if (jumpCount > 0)
    {\
        while (!IsPositionValidAndUnoccupied(jumpPosition))
        {
            jumpPosition = GetRandomPosition(simulationManager.mapSize);
        } 
        cout << "JumpBot ability activated: Jumping to (" << jumpPosition.x << ", " << jumpPosition.y << ")." << endl;
        robot->setPosition(jumpPosition);
        jumpPosition = Position(-1, -1);

        jumpCount--;
    }
    else
    {
        cout << "JumpBot: No more jumps left." << endl;
    }
}

#pragma endregion
