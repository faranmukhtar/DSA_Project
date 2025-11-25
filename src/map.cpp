#include "map.h"
#include "game.h"
#include "allies.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
using namespace std;

Map::Map() {
    WIDTH = sf::VideoMode::getDesktopMode().size.x / 50;
    HEIGHT = sf::VideoMode::getDesktopMode().size.y / 50;

    srand(time(0));

    enemyspawnPoint = GridPosition(35, 7);
    
    dojoPosition = GridPosition(2, 11);

    cout<<"dojo AT "<<dojoPosition.x<<','<<dojoPosition.y<<endl;
    cout<<"enemy AT "<<enemyspawnPoint.x<< ',' <<enemyspawnPoint.y<<endl;
    grid.resize(WIDTH, vector<int>(HEIGHT, 0)); 

    // block edges
    for (int x = 0; x < WIDTH; x++) {
        grid[x][0] = 1;
        grid[x][1] = 1;
        grid[x][HEIGHT - 1] = 1;
        grid[x][HEIGHT - 2] = 1;
    }
    for (int y = 0; y < HEIGHT; y++) {
        grid[0][y] = 1;
        grid[1][y] = 1;
        grid[WIDTH - 1][y] = 1;
        grid[WIDTH - 2][y] = 1;
    }

    // place random obstacles
    srand(time(0));
    grid[32][7] = 2;
    for (int i = 0; i < WIDTH*3; i++) {
        int x = rand() % (WIDTH - 2) + 1;
        int y = rand() % (HEIGHT - 2) + 1;
        if (grid[x][y] == 0 && x != 3 && x != 35 && y!= 7 && y != 11)
            grid[x][y] = 2;
        else
            i--;
    }

    cout << "CALLING FIND" << endl;
    vector<vector<int>> temp1 = grid;
    vector<vector<int>> temp2 = grid;
    vector<vector<int>> temp3 = grid;

    path0.clear();
    path1.clear();
    path2.clear();
  
    findPath(enemyspawnPoint, dojoPosition, 2, temp3);
    findPath(enemyspawnPoint, dojoPosition, 0, temp1);
    findPath(enemyspawnPoint, dojoPosition, 1, temp2);

    vector<vector<bool>> visited(WIDTH, vector<bool>(HEIGHT, false));
    grid[35][enemyspawnPoint.y] = 8;
    grid[2][dojoPosition.y] = 9;

}

GridPosition Map::getenemyspawn() {
    return enemyspawnPoint;
}

vector<vector<int>> Map::getgrid(){
    return grid;
}

GridPosition Map::getRandomSpawnPosition() {
    int gridWidth = getWidth();
    int gridHeight = getHeight();

    cout << "Map dimensions: " << gridWidth << "x" << gridHeight << endl;

    int spawnX = gridWidth - 1;
    int spawnY = gridHeight / 2;

    if (spawnX >= gridWidth) spawnX = gridWidth - 1;
    if (spawnY >= gridHeight) spawnY = gridHeight - 1;
    if (spawnY < 0) spawnY = 0;

    cout << "Spawn position set to: (" << spawnX << ", " << spawnY << endl;
    return GridPosition(spawnX, spawnY);
}

bool Map::isValidPosition(int x, int y) const {
    return (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT);
}

bool Map::isValidPath(int x, int y, vector<vector<int>> &temp) const {
    if (isValidPosition(x, y) && temp[x][y] == 0 ) { 
        
        return true;
    }
    
    return false;
}

bool Map::findPath(const GridPosition &start, const GridPosition &end, int pathfound, vector<vector<int>> &temp) {
    if (!isValidPosition(start.x, start.y))
        return false;

    if (start.x == end.x && start.y == end.y) {
        temp[start.x][start.y] = 3;

        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                if (temp[x][y] == 3)
                 grid[x][y] = 3;
            }
        }
        
        return true;
    }

    if (isValidPath(start.x, start.y, temp) ) {
        temp[start.x][start.y] = 3;

        if(pathfound == 0){
                if (findPath(GridPosition(start.x - 1 , start.y     ), end, pathfound, temp))
                    {path0.push_back(GridPosition(start.x - 1 , start.y    ));return true;}
                if (findPath(GridPosition(start.x     , start.y + 1 ), end, pathfound, temp))
                    {path0.push_back(GridPosition(start.x     , start.y + 1));return true;}
                if (findPath(GridPosition(start.x     , start.y - 1 ), end, pathfound, temp))
                    {path0.push_back(GridPosition(start.x     , start.y - 1));return true;}
                if (findPath(GridPosition(start.x + 1 , start.y     ), end, pathfound, temp))
                    {path0.push_back(GridPosition(start.x + 1  , start.y    ));return true;}
        }

        if(pathfound == 1){
            if (findPath(GridPosition(start.x     , start.y + 1 ), end, pathfound, temp))
                {path1.push_back(GridPosition(start.x     , start.y + 1));return true;}
            if (findPath(GridPosition(start.x - 1 , start.y     ), end, pathfound, temp))
                {path1.push_back(GridPosition(start.x - 1 , start.y    ));return true;}
            if (findPath(GridPosition(start.x     , start.y - 1 ), end, pathfound, temp))
                {path1.push_back(GridPosition(start.x     , start.y - 1));return true;}
            if (findPath(GridPosition(start.x + 1 , start.y     ), end, pathfound, temp)) 
                {path1.push_back(GridPosition(start.x + 1  , start.y    ));return true;}
        }
        if(pathfound == 2){
            if (findPath(GridPosition(start.x     , start.y - 1 ), end, pathfound, temp))
                {path2.push_back(GridPosition(start.x     , start.y - 1));return true;}
            if (findPath(GridPosition(start.x - 1 , start.y     ), end, pathfound, temp)) 
                {path2.push_back(GridPosition(start.x - 1 , start.y    ));return true;}
            if (findPath(GridPosition(start.x     , start.y + 1 ), end, pathfound, temp))
                {path2.push_back(GridPosition(start.x     , start.y + 1));return true;}
            if (findPath(GridPosition(start.x + 1 , start.y     ), end, pathfound, temp))
                {path2.push_back(GridPosition(start.x + 1 , start.y    ));return true;}
        }

        temp[start.x][start.y] = 0;
    }
    
    return false;
}

Map::~Map() {}

int Map::getCellType(int x, int y) const {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        return grid[x][y]; 
    return 1;
}

GridPosition Map::getDojoPosition() const {
    return dojoPosition;
}

int Map::getWidth() const { return WIDTH; }
int Map::getHeight() const { return HEIGHT; }

vector<GridPosition> Map::getoptimumpath(Ally** allies, int count){
    int path0weight = 0, path1weight = 0, path2weight = 0;
    vector<GridPosition> result;
    
    // Calculate weight for path0
    for(int i = 0; i < path0.size(); i++){
        GridPosition pos0 = path0.at(i);
        path0weight += 1;
        
        for(int j = 0; j < count; j++){
            if(allies[j] && allies[j]->getIsActive()){  
                GridPosition allyPos = allies[j]->getPosition();
                int dx = abs(allyPos.x - pos0.x);
                int dy = abs(allyPos.y - pos0.y);
                
                int range = (allies[j]->getType() == 0) ? 1 : 2;
                if(dx <= range && dy <= range){
                    path0weight += 100;
                    cout << "Ally at (" << allyPos.x << "," << allyPos.y 
                         << ") covers path0 tile (" << pos0.x << "," << pos0.y << endl;
                }
            }
        }
    }
    
    // Calculate weight for path1
    for(int i = 0; i < path1.size(); i++){
        GridPosition pos1 = path1.at(i);
        path1weight += 1;
        
        for(int j = 0; j < count; j++){
            if(allies[j] && allies[j]->getIsActive()){
                GridPosition allyPos = allies[j]->getPosition();
                int dx = abs(allyPos.x - pos1.x);
                int dy = abs(allyPos.y - pos1.y);
                
                int range = (allies[j]->getType() == 0) ? 1 : 2;
                if(dx <= range && dy <= range){
                    path1weight += 100;
                    cout << "Ally at (" << allyPos.x << "," << allyPos.y 
                         << ") covers path1 tile (" << pos1.x << "," << pos1.y << endl;
                }
            }
        }
    }
    
    // Calculate weight for path2
    for(int i = 0; i < path2.size(); i++){
        GridPosition pos2 = path2.at(i);
        path2weight += 1;
        
        for(int j = 0; j < count; j++){
            if(allies[j] && allies[j]->getIsActive()){
                GridPosition allyPos = allies[j]->getPosition();
                int dx = abs(allyPos.x - pos2.x);
                int dy = abs(allyPos.y - pos2.y);
                
                int range = (allies[j]->getType() == 0) ? 1 : 2;
                if(dx <= range && dy <= range){
                    path2weight += 100;
                }
            }
        }
    }
    if(path0weight <= path1weight && path0weight <= path2weight){
        cout << "Chosen path0 weight=" << path0weight << endl;
        result = path0;
    }
    else if(path2weight <= path0weight && path2weight <= path1weight){
        cout << "Chosen: path2 weight=" << path2weight << endl;
        result = path2;
    }
    else{
        cout << "Chosen: path1 weight=" << path1weight << endl;
        result = path1;
    }
    
    reverse(result.begin(), result.end());
    return result;
}