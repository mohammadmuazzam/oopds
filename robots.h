#ifndef GENERICROBOT_H   // if GENERICROBOT_H is NOT defined
#define GENERICROBOT_H   // then define it

#include <string>

using namespace std;

class Position
{
    public:
        int x;
        int y;
        Position(int x, int y) : x(x), y(y) {}
};

class GenericRobot
{
    protected:
        Position position;
        Position enemyPosition;
        int numBullets;
        string type = "GenericRobot";
    public:
        string name;
        Position getPosition() const;

        //* only one in 8 neighboring directions
        void look(Position direction);
        void move(int direction);
        void shoot(Position enemyPosition);
};

class MovingRobot : private GenericRobot
{
};




#endif