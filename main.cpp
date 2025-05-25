#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>

#include "robotsAndManager.h"
#include "funcs.h"

using namespace std;

void ReadFromFile(const string& filename);
void HandleRegexPattern(const string& line, int lineNumber);
void PrintMap();
void DefineRobot(smatch matches);
void RandomAction(GenericRobot* robot);

void MoveRobot(GenericRobot* robot);
void LookRobot(GenericRobot* robot);
void ShootRobot(GenericRobot* robot);

string emptySpace = "*";


int main()
{
    string filename = "input.txt";
    ReadFromFile(filename);

    for (int i = 0; i < simulationManager.simulationSteps; i++)
    {
        PrintMap();

        if (simulationManager.robots.size() == 1)
        {
            cout << "One robot left. Ending simulation" << endl;
            break;
        }

        for (const auto& robot : simulationManager.robots)
        {
            RandomAction(robot.get());
        }
    }
    
    cout << "Final map:" << endl;
    PrintMap();
    for (const auto& robot : simulationManager.robots)
    {
        if (robot->getPosition() == Position(-1, -1))
        {
            cout << robot->name << " is dead" << endl;
        }
        else
        {
            cout << robot->name << " is at (" << robot->getPosition().x << ", " << robot->getPosition().y << ")" << endl;
        }   
    }

}

void ReadFromFile(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int lineNumber = 0;
    //* Process each line
    while (getline(file, line))
    {
        HandleRegexPattern(line, lineNumber);
        lineNumber++;
    }

    file.close();
    return;
}

void HandleRegexPattern(const string& line, int lineNumber)
{
    //* Pattern for map size, steps, number of robots, and robot data
    regex patternMapSize(R"(M\s+by\s+N\s*:\s*(\d+)\s+(\d+))");
    regex patternSteps(R"(steps\s*:\s*(\d+))");
    regex patternNumRobots(R"(robots\s*:\s*(\d*))");
    regex patternRobotData(R"((\w+)\s*(\w+)\s*(\d+|random)\s+(\d+|random))");

    smatch matches;

    switch (lineNumber)
    {
        case 0: //* first line must be map size
            if (regex_search(line, matches, patternMapSize))
            {
                simulationManager.mapSize.x = stoi(matches[1]);
                simulationManager.mapSize.y = stoi(matches[2]);
            }
            else
            {
                cerr << "Error: Invalid map size format." << endl;
            }
            break;
        case 1: //* second line must be number of steps
            if (regex_search(line, matches, patternSteps))
            {
                simulationManager.simulationSteps = stoi(matches[1]);
            }
            else
            {
                cerr << "Error: Invalid simulation steps format." << endl;
            }
            break;
        case 2: //* third line must be number of robots
            if (regex_search(line, matches, patternNumRobots))
            {
                simulationManager.numRobots = stoi(matches[1]);
            }
            else
            {
                cerr << "Error: Invalid number of robots format." << endl;
            }
            break;
        default:
            //* remaining lines must be robot data
            if (regex_search(line, matches, patternRobotData))
            {
                DefineRobot(matches);
            }
            else
            {
                cerr << "Error: Invalid robot data format." << endl;
            }
            
            break;
    }
}

void PrintMap()
{
    for (int i = 0; i < simulationManager.mapSize.x; i++)
    {
        cout << "--";
    }
    cout << endl;
    for (int i = 0; i < simulationManager.mapSize.x; i++)
    {
        for (int j = 0; j < simulationManager.mapSize.y; j++)
        {
            bool occupied = false;
            for (const auto& robot : simulationManager.robots)
            {
                //* print first letter of robot name
                if (robot->getPosition().x == i && robot->getPosition().y == j)
                {
                    cout << robot->name[0] << " "; 
                    occupied = true;
                    break;
                }
            }
            if (!occupied)
            {
                //* empty space
                cout << emptySpace <<  " ";
            }
        }
        cout << endl;
    }

    for (int i = 0; i < simulationManager.mapSize.x; i++)
    {
        cout << "--";
    }
    cout << endl;
    
}

void DefineRobot(smatch matches)
{
    string robotType = matches[1];
    string robotName = matches[2];
    Position robotPosition;
    robotPosition.x = (matches[3] == "random") ? GetRandomNumber(0, simulationManager.mapSize.x) : stoi(matches[3]);
    robotPosition.y = (matches[4] == "random") ? GetRandomNumber(0, simulationManager.mapSize.y) : stoi(matches[4]);
    
    //* check if the position is valid
    while (!IsPositionValid(robotPosition, simulationManager.mapSize))
    {
        robotPosition = GetRandomPosition(simulationManager.mapSize);
    }

    GenericRobot* robot;
    //* define robot type
    //* if it is generic robot, then give random type
    if (robotType == "MovingRobot")
    {
        robot = new MovingRobot();
    }
    else
    {
        robot = new GenericRobot();
    }

    robot->name = robotName;
    robot->setPosition(robotPosition);

    simulationManager.robots.push_back(unique_ptr<GenericRobot>(robot));

    return;
}

void RandomAction(GenericRobot* robot)
{
    int action = GetRandomNumber(0, 2);
    cout << "(" << robot->getPosition().x << ", " << robot->getPosition().y << ") ";
    switch (action)
    {
        case 0: //* default move
            robot->move(GetRandomPositionCustom(Position(-1, 1), Position(-1, 1)));
            //#MoveRobot(robot);

            break;
        case 1: //* default look
            robot->look(GetRandomPositionCustom(Position(-1, 1), Position(-1, 1)));
            //#LookRobot(robot);
            break;
        case 2: //* shoot
            robot->shoot(robot->enemyPosition);
            //#ShootRobot(robot);
            break;
    }
}

//* Handle random actions for robots action
void MoveRobot(GenericRobot* robot)
{

}

void LookRobot(GenericRobot* robot)
{

}

void ShootRobot(GenericRobot* robot)
{

}

void ThinkRobot(GenericRobot* robot)
{
    //* Handle thinking logic for robots
    // This function can be used to implement more complex behavior for robots
    // For now, it is left empty as a placeholder
}