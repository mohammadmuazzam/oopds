#pragma once
#include <string>
#include <vector>
#include <memory>

using namespace std;

class Position
{
    public:
        int x;
        int y;
        Position() : x(0), y(0) {}
        Position(int x, int y) : x(x), y(y) {}
};

class GenericRobot
{
    protected:
        Position position;
        Position enemyPosition;
        int numBullets;
    public:
        string name;

        //* only one in 8 neighboring directions
        void look(Position direction);
        void move(int direction);
        void shoot(Position enemyPosition);

        void setPosition(Position newPosition);
        Position getPosition() const;
        virtual string getType();

        void setNumBullets(int bullets);

        virtual ~GenericRobot();
};

class MovingRobot : public GenericRobot
{
    public:
        string getType() override;
};

class SimulationManager
{
    private:
        int simulationTime;
                
    public:
        vector<unique_ptr<GenericRobot>> robots;
        int simulationSteps;
        int numRobots;
        Position mapSize;
};

extern SimulationManager simulationManager;
