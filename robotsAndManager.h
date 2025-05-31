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
    ShootingUpgrade = 1,
    LookingUpgrade = 2,
};

enum class UpgradeName
{
    //* looking upgrades
    ScoutBot,
    TrackingBot,
    //* Shooting Upgrades
    SemiAutoBot,
    ThirtyShellBot,
    LongShotBot,
    //* Moving Upgrades
    HideBot,
    JumpBot
};

#pragma endregion

#pragma region GenericRobot
class GenericRobot
{
    protected:
        Position position;
        string type;
        int numBullets = 10;
        int health = 3;
        bool isVisible = true;
        
    public:
        int moveSteps = 1;
        int lookRange = 1;
        int shootRange = 1;
        string name;
        Position enemyPosition;
        vector<unique_ptr<UpgradeRobot>> upgrades;
        
        void upgradeRandom();

        virtual void look(Position lookPosition);
        virtual void move(Position movePosition);
        virtual void shoot(Position enemyPosition);
        virtual void die();
        void spawn();

        void setPosition(Position newPosition);
        void setNumBullets(int bullets);
        void setIsVisible(bool visible) { isVisible = visible; }

        Position getPosition() const;
        string getType() const;
        int getMoveSteps() const;
        int getLookRange() const;
        int getNumBullets() const { return numBullets; }
        int getShootRange() const { return shootRange; };
        int getHealth() const { return health; }
        bool getIsVisible() const { return isVisible; }

        bool isDead();

        virtual ~GenericRobot();
};
#pragma endregion
#pragma region Robot Types
//* MovingRobot can move at most 2 steps in any direction
class MovingRobot : public GenericRobot
{
    public:
        MovingRobot();
};

class ShootingRobot : public GenericRobot
{
    public:
        ShootingRobot();
        //#void shoot(Position enemyPosition) override;
};

class LookingRobot : public GenericRobot
{
    public:
        LookingRobot();
        //#void look(Position lookPosition) override;
};

class ThinkingRobot : public GenericRobot
{
    protected:
        vector<Position> lastEnemyPositions;
        int lookCount = 0;
        int lookLimit = 0;
        
        //* if saw enemy, then shoot at that position
        //* if not, then think about shooting at random position
    public:
        ThinkingRobot();
        void think();
        void look(Position lookPosition) override;
        
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
#pragma region shooting upgrades
class ScoutRobot : public UpgradeRobot
{
public:
    bool abilityUsed = false;
    vector<Position> enemyPositions;

    ScoutRobot(GenericRobot* owner) { robot = owner; }

    UpgradeName getUpgradeName() override { return UpgradeName::ScoutBot; }
    UpgradeType getUpgradeType() override { return UpgradeType::LookingUpgrade; }

    void upgradedAbility() override;
    const vector<Position>& getEnemyPositions() const { return enemyPositions; }


};

class TrackRobot : public UpgradeRobot
{
protected:
    int trackersRemaining = 3;                       // Can track up to 3 robots
    vector<GenericRobot*> trackedRobots;             // Pointers to tracked enemies

public:
    TrackRobot(GenericRobot* owner) { robot = owner; }

    UpgradeName getUpgradeName() override { return UpgradeName::TrackingBot; }
    UpgradeType getUpgradeType() override { return UpgradeType::LookingUpgrade; }

    void upgradedAbility() override;
    vector<Position> getTrackedPositions() const;
};

class SemiAutoBot : public UpgradeRobot
{
    public:
        UpgradeName getUpgradeName() override { return UpgradeName::SemiAutoBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::ShootingUpgrade; }
        void upgradedAbility() override;
};

class ThirtyShellBot : public UpgradeRobot
{
    public:
        UpgradeName getUpgradeName() override { return UpgradeName::ThirtyShellBot; }
        UpgradeType getUpgradeType() override { return UpgradeType::ShootingUpgrade; }
        void upgradedAbility() override;
};


class LongShotBot : public UpgradeRobot
{
    public:
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
    UpgradeName getUpgradeName() override { return UpgradeName::HideBot; }
    UpgradeType getUpgradeType() override { return UpgradeType::MovingUpgrade; }
    void upgradedAbility() override;
};


class JumpBot : public UpgradeRobot
{
private:
    int jumpCount = 3; // can jump 3 times per match

public:
    UpgradeName getUpgradeName() override { return UpgradeName::JumpBot; }
    UpgradeType getUpgradeType() override { return UpgradeType::MovingUpgrade; }
    void upgradedAbility() override;
};
#pragma endregion

extern SimulationManager simulationManager;
