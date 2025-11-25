#pragma once
#include <random>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

class Ally;

class GridPosition{
public:
    int x;
    int y;
    GridPosition(int x = 0, int y = 0) : x(x), y(y) {}
    
};

class Map{
private:
    int WIDTH ;
    int HEIGHT ;
    vector<vector<int>> grid;
    GridPosition dojoPosition;
    GridPosition enemyspawnPoint;

    vector<GridPosition> path0;
    vector<GridPosition> path1;
    vector<GridPosition> path2;
  
public:
    Map();
    virtual ~Map();
    
    int getCellType(int x, int y) const;
    bool isValidPath(int x, int y, std::vector<std::vector<int>>& tgrid) const ; // looks at the path being genrated to see if block can be part of a possible path
    bool isValidPosition(int x, int y) const; // checks if in map 
    GridPosition getDojoPosition() const;
    
    vector<vector<int>>  getgrid();
    int getWidth() const;
    int getHeight() const;
    bool findPath(const GridPosition& start, const GridPosition& end, int pathfound, std::vector<std::vector<int>>& tempgrid );
    vector<GridPosition> getoptimumpath (Ally ** allies , int count);
    GridPosition getRandomSpawnPosition();
    GridPosition getenemyspawn();

    
};