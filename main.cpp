/************************************
file: main.cpp
Course: Object Oriented Programming & Data Structures
Trimester: 2530
+-------------------------+------------+---------------------------------------------+----------------+
|           NAME          |     ID     |                    EMAIL                    | Phone Number   |
+-------------------------+------------+---------------------------------------------+----------------+
| Mohammad Muazzam Ali    | 242UC244PZ |   mohammad.muazzam.ali@student.mmu.edu.my   |  011-5630 3174 |
| Bin Mohammad Shoaib Ali |            |                                             |                |
+-------------------------+------------+---------------------------------------------+----------------+
| Muhammad Aish Qayyim bin| 242UC244BX |   muhammad.aish.qayyim@student.mmu.edu.my   |  012-342 5662  |
| Mohd Azmi               |            |                                             |                |
+-------------------------+------------+---------------------------------------------+----------------+
*/

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <streambuf>
#include <iomanip>
#include <cmath>

#include "robotsAndManager.h"
#include "funcs.h"

using namespace std;

void ReadFromFile(const string& filename);
void HandleRegexPattern(const string& line, int lineNumber);
void PrintMap();
void DefineRobot(smatch matches);
void RandomAction(GenericRobot* robot);
void Simulation();

void MoveRobot(GenericRobot* robot);
void LookRobot(GenericRobot* robot);
void ShootRobot(GenericRobot* robot);

string emptySpace = ".";
streambuf* originalCoutBuf = cout.rdbuf();
int mapYSizeDigits;


int main()
{
    string filename = "input.txt";
    string outputFilename = "output.txt";
    ofstream outputFile(outputFilename);
    cout.rdbuf(outputFile.rdbuf());
    ReadFromFile(filename);
    mapYSizeDigits = log10(simulationManager.mapSize.y) + 1;

    //* initial print
    for (const auto& robot : simulationManager.robots)
    {
        cout << "Robot: " << robot->name << ", Type: " << robot->getType() 
             << ", Position: (" << robot->getPosition().x << ", " << robot->getPosition().y << ")" << endl;
    }

    //* simulation
    Simulation();
    
    cout << "Final map:" << endl;
    PrintMap();
    for (const auto& robot : simulationManager.robots)
    {
            cout << robot->name << " is at (" << robot->getPosition().x << ", " << robot->getPosition().y << "). Health: " << robot->getHealth() << endl;
    }

    cout << "Dead robots:" << endl;
    for (string deadRobot : simulationManager.deadRobots)
    {
        cout << deadRobot << endl;
    }

    cout.rdbuf(originalCoutBuf); //* restore cout buffers
    cout << "Simulation finished and stored at: " << outputFilename << endl;

}
#pragma region ReadFromFile
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
#pragma endregion
#pragma region HandleRegexPattern
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
                cerr << "Error: Invalid map size format >> " << line << endl;
            }
            break;
        case 1: //* second line must be number of steps
            if (regex_search(line, matches, patternSteps))
            {
                simulationManager.simulationSteps = stoi(matches[1]);
            }
            else
            {
                cerr << "Error: Invalid simulation steps format >> " << line << endl;
            }
            break;
        case 2: //* third line must be number of robots
            if (regex_search(line, matches, patternNumRobots))
            {
                simulationManager.numRobots = stoi(matches[1]);
            }
            else
            {
                cerr << "Error: Invalid number of robots format >> " << line << endl;
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
                cerr << "Error: Invalid robot data format >> " << line << endl;
            }
            
            break;
    }
}
#pragma endregion
#pragma region DefineRobot
void DefineRobot(smatch matches)
{
    string robotType = matches[1];
    string robotName = matches[2];
    Position robotPosition;
    robotPosition.x = (matches[3] == "random") ? GetRandomNumber(1, simulationManager.mapSize.x) : stoi(matches[3]);
    robotPosition.y = (matches[4] == "random") ? GetRandomNumber(1, simulationManager.mapSize.y) : stoi(matches[4]);
    
    //* check if the position is valid
    while (!IsPositionValidAndUnoccupied(robotPosition))
    {
        robotPosition = GetRandomPosition(simulationManager.mapSize);
    }

    GenericRobot* robot;
    //* define robot type
    //* if it is generic robot, then give random type
    if (robotType == "MovingRobot")
    {
        robot = new MovingRobot(robotName, robotPosition);
    }
    else if (robotType == "ShootingRobot")
    {
        robot = new ShootingRobot(robotName, robotPosition);
    }
    else if (robotType == "LookingRobot")
    {
        robot = new LookingRobot(robotName, robotPosition);
    }
    else if (robotType == "ThinkingRobot")
    {
        robot = new ThinkingRobot(robotName, robotPosition);
    }
    else if (robotType == "GenericRobot") //* if it is generic robot, then give random type
    {
        int randomType = GetRandomNumber(0, 2);
        switch (randomType)
        {
            case 0:
                robot = new MovingRobot(robotName, robotPosition);
                break;
            case 1:
                robot = new ShootingRobot(robotName, robotPosition);
                break;
            case 2:
                robot = new LookingRobot(robotName, robotPosition);
                break;
        }
    }

    simulationManager.robots.push_back(unique_ptr<GenericRobot>(robot));

    return;
}
#pragma endregion
#pragma region Simulation
void Simulation()
{
    for (int i = 0; i < simulationManager.simulationSteps; i++)
    {
        cout << "\n!-- STEP " << i + 1 << " --!" << endl;

        if (simulationManager.robots.size() == 1)
        {
            cout << "One robot left. Ending simulation" << endl;
            return;
        }

        //* each robot performs a random action (except thinking robot)
        for (size_t i = 0; i < simulationManager.robots.size(); ++i) 
        {
            auto& robot = simulationManager.robots[i];  

            if (robot->isDead()) 
            {
                simulationManager.robots.erase(simulationManager.robots.begin() + i);
                i--;
            } else 
            {
                RandomAction(robot.get());
            }
        }
        PrintMap();
    }
}
#pragma endregion
#pragma region PrintMap
void PrintMap()
{
    //* print divider line
    for (int i = 0; i < simulationManager.mapSize.x+mapYSizeDigits+3; i++)
    {
        if (i < mapYSizeDigits+2)
            cout << " ";
        else
            cout << "--";
    }
    cout << endl;
    for (int i = 0; i < simulationManager.mapSize.x; i++)
    {
        //* print y coordinate
        cout << setw(mapYSizeDigits) << simulationManager.mapSize.y - i << " | "; //* print y coordinate
        for (int j = 0; j <= simulationManager.mapSize.y; j++)
        {
            if (j == simulationManager.mapSize.y)
            {
                cout << "| ";
                continue;
            }
            bool occupied = false;
            for (const auto& robot : simulationManager.robots)
            {
                //* print robot's avatar at its position
                if (10-robot->getPosition().y == i && robot->getPosition().x-1 == j)
                {
                    cout << robot->avatar << " "; 
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
    //* print divider line
    for (int i = 0; i < simulationManager.mapSize.x+mapYSizeDigits+3; i++)
    {
        if (i < mapYSizeDigits+2)
            cout << " ";
        else
            cout << "--";
    }
    cout << endl;
    //* print x coordinate
    for (int i = 0; i < simulationManager.mapSize.x+mapYSizeDigits+3; i++)
    {
        if (i < mapYSizeDigits+3)
            cout << " ";
        else
            cout << (i - mapYSizeDigits - 2) % 10 << " ";
    }
    cout << endl;    
}
#pragma endregion

#pragma region RandomAction
void RandomAction(GenericRobot* robot)
{
    //* for hidebot
    if (!robot->getIsVisible())
    {
        robot->setIsVisible(true);
        robot->avatar = robot->name[0];
    }
        
    
    cout << "(" << robot->getPosition().x << ", " << robot->getPosition().y << "), " << robot->name << " - ";
    
    //* if robot can think, then think first
    if (robot->getType() == RobotType::ThinkingRobot)
    {
        cout << "THINK" << endl;
        dynamic_cast<ThinkingRobot*>(robot)->think();
        return;
    }

    //* check if robot has been upgraded,
    //* if yes, then random number up to 3
    int maxAction = 2;
    int upgradeIndex = -1;
    if (robot->upgrades.size() > 0)
    {
        upgradeIndex = GetRandomNumber(0, robot->upgrades.size() - 1);
        maxAction = 3;
    }

    int action = GetRandomNumber(0, maxAction);
    
    switch (action)
    {
        case 0: //* default move
            cout << "MOVE" << endl << "\t";
            robot->move(GetRandomPositionCustom(Position(-robot->getMoveSteps(), robot->getMoveSteps())));
            //#MoveRobot(robot);

            break;
        case 1: //* default look
            cout << "LOOK" << endl << "\t";
            robot->look(GetRandomPositionCustom(Position(-robot->getLookRange(), robot->getLookRange())));
            
            //#LookRobot(robot);
            break;
        case 2: //* shoot
            cout << "SHOOT" << endl << "\t";
            robot->shoot(robot->enemyPosition);
            //#ShootRobot(robot);
            break;
        case 3:
            cout << "UPGRADED ABILITY" << endl << "\t";
            robot->upgrades[upgradeIndex]->upgradedAbility();
            break;
        default:
            cout << "UNKNOWN ACTION: " << action << endl;
            break;
    }
    cout << endl;
}
#pragma endregion
