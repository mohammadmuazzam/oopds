#pragma once
#include <string>
#include <vector>
#include <memory>

using namespace std;

class UpgradeRobot;

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

class GenericRobot
{
    protected:
        Position position;
        string type;
        int numBullets;
        int health;
    public:
        string name;
        Position enemyPosition;
        vector<unique_ptr<UpgradeRobot>> upgrades;

        void upgrade(UpgradeRobot* upgrade);

        virtual void look(Position lookPosition);
        virtual void move(Position movePosition);
        virtual void shoot(Position enemyPosition);
        virtual void die();
        void spawn();

        void setPosition(Position newPosition);
        Position getPosition() const;
        string getType();

        void setNumBullets(int bullets);

        GenericRobot();
        virtual ~GenericRobot();
};
//* MovingRobot can move at most 2 steps in any direction
class MovingRobot : public GenericRobot
{
    public:
        MovingRobot();
};

class SimulationManager
{
    private:
        int simulationTime;
                
    public:
        int simulationSteps;
        int numRobots;

        vector<unique_ptr<GenericRobot>> robots;
        Position mapSize;
        GenericRobot* getRobotAtPosition(Position pos);

        bool isPositionOccupied(Position pos);
};

class UpgradeRobot
{
    public:
        virtual string getUpgradeType() = 0;
};

extern SimulationManager simulationManager;
