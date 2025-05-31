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
        bool operator== (const Position& other) const
        {
            bool equal = (x == other.x && y == other.y);
            return equal;
        }

        bool operator!= (const Position& other) const
        {
            return !(*this == other);
        }
        //* position addition
        Position operator+ (const Position& other) const
        {
            return Position(x + other.x, y + other.y);
        }
        //* position addition with int
        Position operator+ (const int value) const
        {
            return Position(x + value, y + value);
        }

        Position operator- (const Position& other) const
        {
            return Position(x - other.x, y - other.y);
        }
        //* position subtraction with int
        Position operator- (const int value) const
        {
            return Position(x - value, y - value);
        }

        void operator+= (const Position& other)
        {
            x += other.x;
            y += other.y;
        }
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
        int moveSteps = 1;
        int lookRange = 1;
        int shootRange = 1;
    public:
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

        Position getPosition() const;
        string getType() const;
        int getMoveSteps() const;
        int getLookRange() const;
        int getShootRange() const { return shootRange; };
        int getHealth() const { return health; }

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
#pragma region Upgrade Robots
class UpgradeRobot
{
    public:
        virtual string getUpgradeType() = 0;
        virtual void upgradedAbility() = 0;
        virtual ~UpgradeRobot() = default;
};

class ScoutRobot : public UpgradeRobot
{
    public:
        bool abilityUsed = false;
    public:
        vector<Position> enemyPositions;
        string getUpgradeType() override;
        //* change abilityUsed after using the ability
        void upgradedAbility() override;
};

class SemiAutoBot : public UpgradeRobot
{
    public:
        string getUpgradeType() override;
        void upgradedAbility() override;
};

class ThirtyShellBot : public UpgradeRobot
{
    public:
        string getUpgradeType() override;
        void upgradedAbility() override;
};


class LongShotBot : public UpgradeRobot
{
    public:
        string getUpgradeType() override;
        void upgradedAbility() override;
};
#pragma endregion

extern SimulationManager simulationManager;
