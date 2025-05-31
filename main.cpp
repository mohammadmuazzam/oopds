#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <streambuf>

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


int main()
{
    string filename = "input.txt";
    string outputFilename = "output.txt";
    ofstream outputFile(outputFilename);
    cout.rdbuf(outputFile.rdbuf());
    ReadFromFile(filename);
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
        robot = new MovingRobot();
    }
    else if (robotType == "ShootingRobot")
    {
        robot = new ShootingRobot();
    }
    else if (robotType == "LookingRobot")
    {
        robot = new LookingRobot();
    }
    else if (robotType == "ThinkingRobot")
    {
        robot = new ThinkingRobot();
    }
    else if (robotType == "GenericRobot") //* if it is generic robot, then give random type
    {
        int randomType = GetRandomNumber(0, 2);
        switch (randomType)
        {
            case 0:
                robot = new MovingRobot();
                break;
            case 1:
                robot = new ShootingRobot();
                break;
            case 2:
                robot = new LookingRobot();
                break;
        }
    }

    robot->name = robotName;
    robot->setPosition(robotPosition);

    simulationManager.robots.push_back(unique_ptr<GenericRobot>(robot));

    return;
}
#pragma endregion
#pragma region Simulation
void Simulation()
{
    for (int i = 0; i < simulationManager.simulationSteps; i++)
    {
        cout << "\nStep " << i + 1 << endl;
        PrintMap();

        if (simulationManager.robots.size() == 1)
        {
            cout << "One robot left. Ending simulation" << endl;
            return;
        }

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
    }
}
#pragma endregion
#pragma region PrintMap
void PrintMap()
{
    for (int i = 0; i <= simulationManager.mapSize.x; i++)
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
                if (robot->getPosition().x-1 == i && robot->getPosition().y-1 == j)
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
#pragma endregion

#pragma region RandomAction
void RandomAction(GenericRobot* robot)
{
    //* for hidebot
    if (!robot->getIsVisible())
        robot->setIsVisible(true);
    
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
        return;
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
