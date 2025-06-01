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
        case UpgradeName::TrackBot: os << "TrackingBot"; break;
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
    cout << "trying to move to (" << newPosition.x << ", " << newPosition.y << ")" << endl;

    if (!IsPositionValidAndUnoccupied(newPosition))
    {
        cout << "\t(" << newPosition.x << ", " << newPosition.y << ") is occupied or out of bounds. Cannot move there." << endl;
        return;
    }

    position = newPosition;
    cout << "\tsuccessfully moved to (" << position.x << ", " << position.y << ")" << endl;    
}

void GenericRobot::look(Position lookPosition)
{
    cout << "looked at (" << lookPosition.x + position.x << ", " << lookPosition.y + position.y << ")" << endl;
    
    for (const auto& robot : simulationManager.robots)
    {
        if (robot->getPosition() == position + lookPosition)
        {
            enemyPosition = robot->getPosition();
            return;
        }
    }
    
    enemyPosition = Position(-1, -1);
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
    
    if (enemyPosition != Position(-1, -1) && GetShootDistance(position, enemyPosition) <= shootRange)
        shootPosition = enemyPosition;
    else
        shootPosition = GetRandomPositionCustom(Position(-shootRange, shootRange), Position(-shootRange, shootRange)) + position;
    
    if (simulationManager.getRobotAtPosition(shootPosition) != nullptr)
    {
        GenericRobot* enemyRobot = simulationManager.getRobotAtPosition(shootPosition);
        cout << "\tTARGET ACQUIRED: " << enemyRobot->name << endl;
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
        cout << "\tNO ROBOT (" << shootPosition.x << ", " << shootPosition.y << ")" << endl;
    }

    numBullets--;

    // check if out of bullets
    if (numBullets <= 0)
    {
        cout << "\t" << getType() << "-" << name << " ran out of bullets" << endl;
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
        //#cout << "\tAlready applied upgrade: " << upgrade->getUpgradeName() << "(" << upgrade->getUpgradeType() << ")" << endl;
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

GenericRobot::GenericRobot(string robotName, Position robotPos)
{
    name = robotName;
    avatar = robotName[0];
    position = robotPos;
}

GenericRobot::GenericRobot()
{
    
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

MovingRobot::MovingRobot(string robotName, Position robotPos)
            : GenericRobot(robotName, robotPos)
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

ShootingRobot::ShootingRobot(string robotName, Position robotPos)
            : GenericRobot(robotName, robotPos)
{
    type = RobotType::ShootingRobot;
    numBullets = 15; 
}

#pragma endregion

#pragma region LookingRobot

LookingRobot::LookingRobot()
{
    type = RobotType::LookingRobot;
    lookRange = 2; 
}

LookingRobot::LookingRobot(string robotName, Position robotPos)
            : GenericRobot(robotName, robotPos)
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

ThinkingRobot::ThinkingRobot(string robotName, Position robotPos)
            : GenericRobot(robotName, robotPos)
{
    type = RobotType::ThinkingRobot;
}

void ThinkingRobot::think()
{
    //** immediately activate longshotbot or thirtyshellbot if available
    if (!shootingUpgradeActivated)
    {
        for (const auto &upgrade : upgrades)
        {
            if (upgrade->getUpgradeName() == UpgradeName::LongShotBot || 
                upgrade->getUpgradeName() == UpgradeName::ThirtyShellBot)
            {
                upgrade->upgradedAbility();
                shootingUpgradeActivated = true;
            }
        } 
    }    

    if (lookLimit == 0)
    {
        lookLimit = GetRandomNumber(1, 3);
    }
    
    //* if no data about enemy positions, then look around
    if (lastEnemyPositions.empty() && lookCount <= lookLimit)
    {
        cout << "\t... No known enemy positions. Looking around." << endl;

        //* generate random look direction based on current position that is in bounds
        Position lookDirection = GetRandomPositionInBounds(position, lookRange);
        
        look(lookDirection);
        if (lastEnemyPositions.empty())
        {
            cout << "\t... No enemy found in sight." << endl;
        }
        return;
    }
    
    //* shoot randomly 10% of the time
    if (ProbabilityCheck(10) && numBullets > 3)
    {
        cout << "\t... Shooting at random position." << endl;
        Position randomShootPosition = GetRandomPositionCustom(Position(-shootRange, shootRange), Position(-shootRange, shootRange)) + position;
        shoot(randomShootPosition);
    }

    //* if lookCount exceeds a random number and still no data about enemy, then move randomly
    if (lookCount > lookLimit && lastEnemyPositions.empty())
    {
        lookCount = 0;
        lookLimit = 0; 
        cout << "\t... Moving randomly." << endl;
        Position movePos = GetRandomPositionInBounds(position, moveSteps);
        move(movePos);
        return;
    }

    //* if last enemy position isn't in shooting range, 
    //* then move towards closest known enemy position
    if (!lastEnemyPositions.empty())
    {
        Position closestEnemyPosition = GetClosestPosition(position, lastEnemyPositions);
        if (GetShootDistance(position, closestEnemyPosition) > shootRange)
        {
            cout << "\t... " << GetShootDistance(position, closestEnemyPosition) << " >? " << shootRange << endl;
            Position movePosition = closestEnemyPosition - position;
            //##cout << "\t... vector to closest enemy position: " << movePosition << endl;
            if (abs(movePosition.x) > moveSteps) movePosition.x = movePosition.x > 0 ? moveSteps : -moveSteps;
            if (movePosition.y > moveSteps) movePosition.y = movePosition.y > 0 ? moveSteps : -moveSteps;
            cout << "\t... Moving towards last known enemy position - " << closestEnemyPosition << "\n";
            //#cout << "\t... Valid moving vector: " << movePosition << endl;
            move(movePosition);
            return;
        }
        else
        {
            //* either shoot or hide (50% chance)
            bool canHide = false;
            for (const auto &upgrade : upgrades)
            {
                if (upgrade->getUpgradeName() == UpgradeName::HideBot && rand() % 2 == 0)
                {
                    upgrade->upgradedAbility();
                    canHide = true;
                }
            }

            if (!canHide)
            {
                cout << "\t... In range with last known enemy position. Shooting." << endl;
                shoot(closestEnemyPosition);
                //* remove the closest enemy position from the list
                lastEnemyPositions.erase(remove(lastEnemyPositions.begin(), lastEnemyPositions.end(), closestEnemyPosition), lastEnemyPositions.end());    
            }
            return;
        }
    }

}

void ThinkingRobot::look(Position lookDirection)
{
    lookCount++;

    //* if scoutbot ability is available, then look at whole map
    for (const auto &upgrade : upgrades)
    {
        if (upgrade->getUpgradeName() == UpgradeName::ScoutBot)
        {
            cout << "\tScoutBot ability activated. Looking at the entire map.\n\t";
            upgrade->upgradedAbility();
            
            ScoutRobot* scout = dynamic_cast<ScoutRobot*>(upgrade.get());
            if (scout)
            {
                lastEnemyPositions = scout->enemyPositions;
            }
            else
            {
                cout << "Error: ScoutRobot upgrade is not of type ScoutRobot." << endl;
            }
            return;
        }
        //* if no scoutbot upgrade, then use trackbot.
        //* if no tracked robot, add one. else, get tracked positions.
        else if (upgrade->getUpgradeName() == UpgradeName::TrackBot)
        {
            cout << "\tTrackBot ability activated. Tracking enemies." << endl;
            TrackRobot* track = dynamic_cast<TrackRobot*>(upgrade.get());
            if (track)
            {
                if (track->getTrackedPositions().empty())
                {
                    cout << "\tNo enemies currently being tracked. Adding tracker." << endl;
                    track->addTracker();
                }                

                if (!track->getTrackedPositions().empty())
                    lastEnemyPositions = track->getTrackedPositions();
            }
            else
            {
                cout << "Error: TrackRobot upgrade is not of type TrackRobot." << endl;
            }
            return;
        }
    }

    //* if no scoutbot or trackbot upgrade, then look in the specified direction
    cout << "\t";
    GenericRobot::look(lookDirection);
    
    if (enemyPosition != Position(-1, -1))
    {
        cout << "\tEnemy found at (" << enemyPosition.x << ", " << enemyPosition.y << ")" << endl;
        lastEnemyPositions.push_back(enemyPosition);
    }
}

void ThinkingRobot::move(Position moveDistance)
{
    cout << "\t... ";

    //* use jumpbot ability if available
    for (const auto& upgrade : upgrades)
    {
        if (upgrade->getUpgradeName() == UpgradeName::JumpBot)
        {
            JumpBot* jumpBot = dynamic_cast<JumpBot*>(upgrade.get());
            //* find valid neighboring positions near enemy to jump to
            if (!lastEnemyPositions.empty())
            {
                vector<Position> neighboringEnemyPositions = GetAllNeighborOffsetExtension(GetClosestPosition(this->position, lastEnemyPositions));
                vector<Position> validNeighboringEnemyPositions;
                cout << "JumpBot ability: checking neighboring enemy positions." << endl;
                for (const auto& pos : neighboringEnemyPositions)
                {
                    if (IsPositionValidAndUnoccupied(pos))
                    {
                        validNeighboringEnemyPositions.push_back(pos);
                        cout << "\t\t" << pos << " is valid" << endl;
                    }
                    else
                        cout << "\t\t" << pos << " is invalid" << endl;
                }

                if (validNeighboringEnemyPositions.empty())
                {
                    cout << "\tNo valid neighboring enemy positions to jump to. Jumping to random position." << endl;
                    jumpBot->jumpPosition = Position(-1, -1);
                }
                else
                {
                    Position jumpPosition = validNeighboringEnemyPositions[GetRandomNumber(0, validNeighboringEnemyPositions.size() - 1)];
                    cout << "\tJumping to a random neighboring position: " << jumpPosition << endl;
                    jumpBot->jumpPosition = jumpPosition;
                }
            }
            cout << "\tJumpBot ability activated. Jumping to a random position.\n\t";
            upgrade->upgradedAbility();
            return;
        }
    }

    GenericRobot::move(moveDistance);

    //* check if the move was successful
    //* if not, maybe there's an enemy there
    if (position == moveDistance + position)
    {
        lastEnemyPositions.push_back(moveDistance + position);
    }
    
}

void ThinkingRobot::shoot(Position enemyPosition)
{
    cout << "\t... ";
    //! HERE
    //* if semi auto bot upgrade is available, use it 50% of the time
    if (numBullets > 4 && ProbabilityCheck(50))
    {
        for (const auto& upgrade : upgrades)
        {
            if (upgrade->getUpgradeName() == UpgradeName::SemiAutoBot)
            {
                SemiAutoBot* semiAutoBot = dynamic_cast<SemiAutoBot*>(upgrade.get());
                semiAutoBot->upgradedAbility();
                return;
            }
        }
    }
    else
        GenericRobot::shoot(enemyPosition);
}

#pragma endregion


SimulationManager simulationManager;

#pragma region Shooting Upgrades

void LongShotBot::upgradedAbility()
{
    if (abilityUsed)
    {
        cout << "\tLongShotBot ability already used." << endl;
        return;
    }
    cout << "\tLongShotBot ability activated: Can shoot targets where x + y <= 3." << endl;
    robot->setShootRange(3);
    abilityUsed = true;
    /* 
    for (auto& robot : simulationManager.robots)
    {
        if (find_if(robot->upgrades.begin(), robot->upgrades.end(), [](const unique_ptr<UpgradeRobot>& upgrade) {
            return upgrade->getUpgradeName() == UpgradeName::LongShotBot;
        }) != robot->upgrades.end())
        {
            robot->shootRange = 3;
        }
    }
    */
}

void SemiAutoBot::upgradedAbility()
{
    
    Position target = robot->enemyPosition;
    if (target == Position(-1, -1))
    {
        target = GetRandomPositionCustom(Position(-robot->getShootRange(), robot->getShootRange()), Position(-robot->getShootRange(), robot->getShootRange())) + robot->getPosition();
    }

    cout << "SemiAutoBot ability activated. Shooting at " << target << endl;

    GenericRobot* targetRobot = simulationManager.getRobotAtPosition(target);
    if (targetRobot != nullptr)
    {
        for (int i = 0; i < 3; ++i)
        {
            cout << "\t\tATTEMPT " << i + 1 << " at " << target << endl;
            if (ProbabilityCheck(70))
            {
                cout << "\tSEMI-AUTO HIT " << targetRobot->getType() << ", " << targetRobot->name << endl;
                targetRobot->die();
                robot->upgradeRandom();
                break;
            }
            else
            {
                cout << "\tSEMI-AUTO MISS " << targetRobot->getType() << ", " << targetRobot->name << endl;
            }
        }
    }
    else
    {
        cout << "\tSEMI-AUTO NO TARGET at " << target << endl;
    }

    robot->setNumBullets(robot->getNumBullets() - 3);
    if (robot->getNumBullets() <= 0)
    {
        cout << "\t" << robot->getType() << "-" << robot->name << " ran out of bullets" << endl;
        robot->die();
    }
}


void ThirtyShellBot::upgradedAbility()
{
    if (abilityUsed) {
        cout << "\tThirtyShellBot ability already used." << endl;
        return;
    }
    cout << "\tThirtyShellBot ability activated: Ammo reloaded to 30 shells." << endl;
    robot->setNumBullets(30);
    abilityUsed = true;

    /*
    for (auto& robot : simulationManager.robots)
    {
        if (find_if(robot->upgrades.begin(), robot->upgrades.end(), [](const unique_ptr<UpgradeRobot>& upgrade) {
            return upgrade->getUpgradeName() == UpgradeName::ThirtyShellBot;
        }) != robot->upgrades.end())
        {
            robot->setNumBullets(30);
        }
    }
    */
}
#pragma endregion

#pragma region Looking Upgrades
void ScoutRobot::upgradedAbility() 
{
    if (abilityUsed) {
        cout << "Ability already used." << endl;
        return;
    }

    cout << "Scanning entire battlefield..." << endl;

    enemyPositions.clear();

    //Loop through all robots in the simulation
    for (const auto& r : simulationManager.robots) 
    {
        if (r.get() != robot && !r->isDead()) 
        {
            Position enemyPos = r->getPosition();
            enemyPositions.push_back(enemyPos);

            cout << "\tEnemy: " << r->name << " at (" << enemyPos.x << ", " << enemyPos.y << ")" << endl;
        }
    }
    //* for non-thinking robots, set enemy position to closest enemy
    if (robot->getType() != RobotType::ThinkingRobot && !enemyPositions.empty()) 
    {
        Position closestEnemy = GetClosestPosition(robot->getPosition(), enemyPositions);
        robot->enemyPosition = closestEnemy;
        cout << "\tClosest enemy position set to: " << closestEnemy << endl;
    }
    abilityUsed = true;
}

void TrackRobot::upgradedAbility() //* for non-thinking robots
{
    //* 50% chance to add a tracker or get data from tracker (for non thinking  robot)
    if (ProbabilityCheck(50)) 
    {
        addTracker();
    } 
    else 
    {
        cout << "\tTrackBot ability: Retrieving tracked enemy positions..." << endl;
        vector<Position> trackedPositions = getTrackedPositions();
        if (trackedPositions.empty()) 
        {
            cout << "\t\tNo enemies currently being tracked." << endl;
        } 
        else 
        {
            cout << "\t\tCurrently tracked enemy positions:" << endl;
            for (const auto& pos : trackedPositions) 
            {
                cout << "\t\t" << pos << endl;
            }
            Position closestEnemy = GetClosestPosition(robot->getPosition(), trackedPositions);
            robot->enemyPosition = closestEnemy;
        }
    }
}

void TrackRobot::addTracker()
{
    if (trackersRemaining <= 0) 
    {
        cout << "\tTrackBot ability: No more trackers available." << endl;
        return;
    }

    cout << "\tTrackBot ability: Scanning nearby for enemies..." << endl;

    //Allow the robot to check surrounding
    vector<Position> offsets = GetAllNeighborOffset(Position(-robot->getLookRange(), robot->getLookRange()));
    

    for (const auto& offset : offsets) {
        Position checkPos = robot->getPosition() + offset;
        GenericRobot* target = simulationManager.getRobotAtPosition(checkPos);

        if (target && target != robot && !target->isDead()) {
            if (std::find(trackedRobots.begin(), trackedRobots.end(), target) == trackedRobots.end()) {
                trackedRobots.push_back(target);
                trackersRemaining--;
                cout << "\tTrackBot ability: Tracker placed on: " << target->name << endl;
                return;
            }
        }
    }

    cout << "\tTrackBot ability: No valid nearby enemy to track." << endl;
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
        robot->avatar = '-';
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
