#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>

#include "robotsAndManager.h"

using namespace std;

void ReadFromFile(const string& filename);
void HandleRegexPattern(const string& line, int lineNumber);
int getRandomNumber(int min, int max);
Position getRandomPosition(int min, int max);

//! UNDEFINED REFERENCE TO ALL OF THEM

int main()
{
    string filename = "input.txt";
    ReadFromFile(filename);

    cout << "Map Size: " << simulationManager.mapSize.x << " by " << simulationManager.mapSize.y << endl;
    cout << "Simulation Steps: " << simulationManager.simulationSteps << endl;
    cout << "Number of Robots: " << simulationManager.numRobots << endl;
    cout << "Robots:" << endl;
    for (const auto& robot : simulationManager.robots)
    {
        cout << "Robot Name: " << robot->name << ", Type: " << robot->getType() << endl;
        cout << "Position: (" << robot->getPosition().x << ", " << robot->getPosition().y << ")" << endl;
    }

    cout << "Robot 0:" << simulationManager.robots[0]->name << endl;

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
            if (regex_search(line, matches, patternRobotData))
            {
                string robotType = matches[1];
                string robotName = matches[2];
                Position robotPosition;
                robotPosition.x = (matches[3] == "random") ? getRandomNumber(0, simulationManager.mapSize.x) : stoi(matches[3]);
                robotPosition.y = (matches[4] == "random") ? getRandomNumber(0, simulationManager.mapSize.y) : stoi(matches[4]);

                GenericRobot* robot;
                
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
            }
            else
            {
                cerr << "Error: Invalid robot data format." << endl;
            }
            
            break;
    }


}

Position getRandomPosition(int min, int max) {
    Position pos;
    pos.x = rand() % (max - min + 1) + min;
    pos.y = rand() % (max - min + 1) + min;
    return pos;
}

int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}