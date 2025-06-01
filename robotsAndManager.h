/*
file: robotsAndManager.h
Course: Object Oriented Programming & Data Structures
Trimester: 2510
+===========================+============+=============================================+================+
|           NAME            |     ID     |                    EMAIL                    | Phone Number   |
+===========================+============+=============================================+================+
| Mohammad Muazzam Ali      | 242UC244PZ |   mohammad.muazzam.ali@student.mmu.edu.my   |  011-5630 3174 |
| Bin Mohammad Shoaib Ali   |            |                                             |                |
+---------------------------+------------+---------------------------------------------+----------------+
| Muhammad Aish Qayyim bin  | 242UC244BX |   muhammad.aish.qayyim@student.mmu.edu.my   |  012-342 5662  |
| Mohd Azmi                 |            |                                             |                |
+---------------------------+------------+---------------------------------------------+----------------+
| Muhammad Aref bin Mohd    | 242UC244Q1 |    muhammad.aref.mohd@student.mmu.edu.my    |  010-650 9597  |
| Hasni                     |            |                                             |                |
+---------------------------+------------+---------------------------------------------+----------------+
| Muhammad Fahmi bin Hanafi | 242UC244FX |   muhammad.fahmi.hanafi@student.mmu.edu.my  + 011-3174 3545  |
+===========================+============+=============================================+================+
*/

#pragma once
#include <string>
#include <vector>
#include <memory>

using namespace std;

class UpgradeRobot;

#pragma region Position
class Position
{
    public:
        int x;
        int y;
        Position() : x(-1), y(-1) {}
        Position(int x, int y) : x(x), y(y) {}

        //* check equality and non-equality
        bool operator== (const Position& other) const { return (x == other.x && y == other.y); }

        bool operator!= (const Position& other) const { return !(*this == other); }
        
        //* position addition
        Position operator+ (const Position& other) const { return Position(x + other.x, y + other.y); }
        //* position addition with int
        Position operator+ (const int value) const { return Position(x + value, y + value); }

        Position operator- (const Position& other) const { return Position(x - other.x, y - other.y); }
        //* position subtraction with int
        Position operator- (const int value) const { return Position(x - value, y - value); }

        void operator+= (const Position& other) { *this = *this + other; }
};
#pragma endregion

#pragma region enums
enum class RobotType
{
    GenericRobot,
    MovingRobot,
    ShootingRobot,
    LookingRobot,
    ThinkingRobot
};

enum class UpgradeType
{
    MovingUpgrade = 0,
    LookingUpgrade = 1,
    ShootingUpgrade = 2    
};

enum class UpgradeName
{
    //* looking upgrades
    ScoutBot,
    TrackBot,
    //* Shooting Upgrades
    SemiAutoBot,
    ThirtyShellBot,
    LongShotBot,
    //* Moving Upgrades
    HideBot,
    JumpBot
};

#pragma endregion

ostream& operator<<(ostream& os, const Position& pos);
ostream& operator<<(ostream& os, const RobotType& type);
ostream& operator<<(ostream& os, const UpgradeName& name);
ostream& operator<<(ostream& os, const UpgradeType& type);

#pragma region GenericRobot
class GenericRobot
{
    protected:
        Position position;
        RobotType type;
        int numBullets = 10;
        int health = 3;
        bool isVisible = true;
        
    public:
        int moveSteps = 1;
        int lookRange = 1;
        int shootRange = 1;
        string name;
        char avatar;
        Position enemyPosition;
        vector<unique_ptr<UpgradeRobot>> upgrades;
        

        virtual void look(Position lookPosition);
        virtual void move(Position movePosition);
        virtual void shoot(Position enemyPosition);
        void die();

        void spawn();
        void upgradeRandom();

        void setPosition(Position newPosition) { position = newPosition; };
        void setNumBullets(int bullets) { numBullets = bullets; };
        void setIsVisible(bool visible) { isVisible = visible; }
        void setShootRange(int range) { shootRange = range; }

        Position getPosition() const { return position; }
        RobotType getType() const { return type; }
        int getMoveSteps() const { return moveSteps; }
        int getLookRange() const { return lookRange; }
        int getNumBullets() const { return numBullets; }
        int getShootRange() const { return shootRange; };
        int getHealth() const { return health; }
        bool getIsVisible() const { return isVisible; }

        bool isDead() const { return health <= 0; }

        GenericRobot();
        GenericRobot(string robotName, Position robotPos);
        virtual ~GenericRobot();
};
#pragma endregion
#pragma region Robot Types
//* MovingRobot can move at most 2 steps in any direction
class MovingRobot : public GenericRobot
{
    public:
        MovingRobot();
        MovingRobot(string robotName, Position robotPos);
};

class ShootingRobot : public GenericRobot
{
    public:
        ShootingRobot();
        ShootingRobot(string robotName, Position robotPos);
        //#void shoot(Position enemyPosition) override;
};

class LookingRobot : public GenericRobot
{
    public:
        LookingRobot();
        LookingRobot(string robotName, Position robotPos);
        //#void look(Position lookPosition) override;
};

class ThinkingRobot : public GenericRobot
{
    protected:
        vector<Position> lastEnemyPositions;
        int lookCount = 0;
        int lookLimit = 0;
        bool shootingUpgradeActivated = false; // if longshotbot or thirtyshellbot is activated

    public:
        ThinkingRobot();
        ThinkingRobot(string robotName, Position robotPos);
        void think();
        void look(Position lookPosition) override;
        void move(Position movePosition) override;
        void shoot(Position enemyPosition) override;
        
};
#pragma endregion

#pragma region SimulationManager
class SimulationManager
{
    private:
        int simulationTime;

    public:
        int simulationSteps;
        int numRobots;

        vector<unique_ptr<GenericRobot>> robots;
        vector<string> deadRobots;
        Position mapSize;
        GenericRobot* getRobotAtPosition(Position pos);

        //#bool isPositionOccupied(Position pos);
};
#pragma endregion

class UpgradeRobot
{
    public:
        GenericRobot* robot;
        virtual UpgradeType getUpgradeType() = 0;
        virtual UpgradeName getUpgradeName() = 0;
        virtual void upgradedAbility() = 0;
        virtual ~UpgradeRobot() = default;
};
#pragma region looking upgrades
class ScoutRobot : public UpgradeRobot
{
    public:
        bool abilityUsed = false;
        vector<Position> enemyPositions;

        ScoutRobot(GenericRobot* owner) { robot = owner; }
        ScoutRobot() = default;

        UpgradeName getUpgradeName() override { return UpgradeName::ScoutBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::LookingUpgrade; }

        void upgradedAbility() override;
        const vector<Position>& getEnemyPositions() const { return enemyPositions; }


};

class TrackRobot : public UpgradeRobot
{
    private:
        int trackersRemaining = 3;                       // Can track up to 3 robots
        vector<GenericRobot*> trackedRobots;             // Pointers to tracked enemies

    public:
        TrackRobot(GenericRobot* owner) { robot = owner; }
        TrackRobot() = default;

        UpgradeName getUpgradeName() override { return UpgradeName::TrackBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::LookingUpgrade; }

        void upgradedAbility() override;
        vector<Position> getTrackedPositions() const;
        void addTracker();
};
#pragma endregion
#pragma region shooting upgrades
class SemiAutoBot : public UpgradeRobot
{
    public:
        SemiAutoBot(GenericRobot* owner) { robot = owner; }
        SemiAutoBot() = default;
        UpgradeName getUpgradeName() override { return UpgradeName::SemiAutoBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::ShootingUpgrade; }
        void upgradedAbility() override;
};

class ThirtyShellBot : public UpgradeRobot
{
    public:
        bool abilityUsed = false;

        ThirtyShellBot(GenericRobot* owner) { robot = owner; }
        ThirtyShellBot() = default;
        UpgradeName getUpgradeName() override { return UpgradeName::ThirtyShellBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::ShootingUpgrade; }
        void upgradedAbility() override;
};


class LongShotBot : public UpgradeRobot
{
    public:
        bool abilityUsed = false;

        LongShotBot(GenericRobot* owner) { robot = owner; }
        LongShotBot() = default;
        UpgradeName getUpgradeName() override { return UpgradeName::LongShotBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::ShootingUpgrade; }
        void upgradedAbility() override;
};
#pragma endregion
#pragma region moving upgrades
class HideBot : public UpgradeRobot
{
    private:
        int hideCount = 3; // can hide 3 times per match

    public:
        HideBot(GenericRobot* owner) { robot = owner; }
        HideBot() = default;
        UpgradeName getUpgradeName() override { return UpgradeName::HideBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::MovingUpgrade; }
        void upgradedAbility() override;
};


class JumpBot : public UpgradeRobot
{
    private:
        int jumpCount = 3; // can jump 3 times per match
        

    public:
        Position jumpPosition = Position(-1, -1); // position to jump to
        JumpBot(GenericRobot* owner) { robot = owner; }
        JumpBot() = default;
        UpgradeName getUpgradeName() override { return UpgradeName::JumpBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::MovingUpgrade; }
        void upgradedAbility() override;
};
#pragma endregion

extern SimulationManager simulationManager;
