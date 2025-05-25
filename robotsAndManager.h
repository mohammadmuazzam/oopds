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
};

class GenericRobot
{
    protected:
        Position position;
        int numBullets;
        int health;
    public:
        string name;
        Position enemyPosition;
        vector<unique_ptr<UpgradeRobot>> upgrades;

        void upgrade(UpgradeRobot* upgrade);

        //* only one in 8 neighboring directions
        //* we use 0-7 to represent the 8 directions
        //* 0 1 2
        //* 3   4
        //* 5 6 7
        virtual void look(int direction);
        virtual void move(int direction);
        virtual void shoot(Position enemyPosition);
        virtual void die();
        void spawn();

        void setPosition(Position newPosition);
        Position getPosition() const;
        virtual string getType();

        void setNumBullets(int bullets);

        GenericRobot();
        virtual ~GenericRobot();
};

class MovingRobot : public GenericRobot
{
    public:
        string getType() override;
        void move(int direction) override;
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

};

extern SimulationManager simulationManager;
