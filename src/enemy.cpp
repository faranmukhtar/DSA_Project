#include "enemy.h"
#include "allies.h"
#include <cmath>

Enemy::Enemy(int type, int health, int damage, double speed, GridPosition spawnPos , GridPosition dojo) 
    : type(type), health(health), maxHealth(health), damage(damage), speed(speed), 
      currentPosition(spawnPos), path(nullptr), isMoving(true), pathLength(0), currentPathIndex(0), 
      isActive(true), reachedDojo(false) , spawnposition(spawnPos) , dojopos(dojo) ,animationTimer(0.0f), currentAnimFrame(0),
      isAttacking(false), currentTarget(nullptr), attackCooldown(1.0f), attackTimer(0.0f){
        sf::Clock deltaClock ;
        double dTime = deltaClock.restart().asSeconds(); 
        
        pixelPosition.x = spawnPos.x * 40;  
        pixelPosition.y = spawnPos.y * 40;
      }

Enemy::~Enemy() {
    delete[] path;
}

bool Enemy::getismoving() const{
    return isMoving;
}

void Enemy::update(double deltaTime, Map * map , Ally** allies, Dojo* dojo, int count){
    if (!isActive) return;

    moveAlongPath(deltaTime, map->getgrid());

    checkForTargets(allies, count, dojo);

        if(isAttacking){
            attackTarget(deltaTime, dojo);
            animationTimer += deltaTime;
            if(animationTimer >= 0.3f) {
                currentAnimFrame = (currentAnimFrame + 1) % 5;
                animationTimer = 0.0f;
            }
        }
        else{
            animationTimer += deltaTime;
            if(animationTimer >= 0.6f) {
                currentAnimFrame = (currentAnimFrame + 1) % 5;
                animationTimer = 0.0f;
            }
        }
}

void Enemy::checkForTargets(Ally** allies, int allyCount, Dojo* dojo){
    for(int i = 0; i < allyCount; i++){
        if(allies[i] && allies[i]->getIsActive() && allies[i]->getType() == 0){ //samurais are prioritized over dojo
            if(isSamuraiInRange(allies[i])){
                currentTarget = allies[i];
                isAttacking = true;
                isMoving = false;
                return;
            }
        }
    }
    
    if(reachedDojo && dojo && !dojo->isDojoDestroyed()){
        currentTarget = nullptr;
        isAttacking = true;
        isMoving = false;
        return;
    }
    
    if(currentTarget || isAttacking){
        currentTarget = nullptr;
        isAttacking = false;
        isMoving = true;
    }
}

bool Enemy::getisAttacking() const {
    return isAttacking;
}

void Enemy::attackTarget(float deltaTime, Dojo* dojo){
    if (!isAttacking) return;
    
    attackTimer -= deltaTime;
    
    if(attackTimer <= 0.0f){
        if(currentTarget){
            Samurai* samurai = dynamic_cast<Samurai*>(currentTarget);
            if(samurai && samurai->getIsActive() && isSamuraiInRange(samurai)){
                samurai->takeDamage(damage);
                isMoving = false;
            }
            else{
                currentTarget = nullptr;
                if(!reachedDojo){
                    isAttacking = false;
                    isMoving = true;
                }
            }
        } 
        else if(reachedDojo && dojo && !dojo->isDojoDestroyed()){
            cout << "attacking dojo, damage: " << damage << endl;
            dojo->takeDamage(damage);
            
        }
        attackTimer = attackCooldown;
    }
}

bool Enemy::isSamuraiInRange(Ally* samurai) const{
    if(!samurai || !samurai->getIsActive()) return false;
    
    sf::Vector2f samuraiPos = samurai->getPixelPos();
    float dx = pixelPosition.x - samuraiPos.x;
    float dy = pixelPosition.y - samuraiPos.y;
    float distance = std::sqrt(dx*dx + dy*dy);
    
    return distance <= 40.0f; 
}

int Enemy::getAnimationFrame() const { 
    return currentAnimFrame; 
}

void Enemy::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        isActive = false;
    }
}

void Enemy::moveAlongPath(float deltaTime, vector<vector<int>> grid){
   
    if(!isMoving){ 
        //cout << "Not moving" << endl; 
        return; 
    }

    float moveAmount = speed * deltaTime * 2.0f;
    
    pathLength = path->size();
    for(int i = 0 ; i< pathLength ; i++){
        // cout<<"Inside"<<endl;
        GridPosition target = path->at(i);
        // cout<<"at "<<i<<" "<<target.x<<" ,"<<target.y<<endl;
    }
    
    if(!path->empty() && currentPathIndex < pathLength){
        // cout<<"PATH IS NOT EMPTY"<<endl;
        GridPosition target = path->at(currentPathIndex);
        int TargetPixelX = target.x * 40;
        int TargetPixelY = target.y * 40;

        float dx = TargetPixelX - pixelPosition.x;
        float dy = TargetPixelY - pixelPosition.y;

        bool reachedX = fabs(dx) <= moveAmount;
        bool reachedY = fabs(dy) <= moveAmount;

        

        if (!reachedX)
            pixelPosition.x += (dx > 0 ? moveAmount : -moveAmount);
        else
            pixelPosition.x = TargetPixelX;

        if (!reachedY)
            pixelPosition.y += (dy > 0 ? moveAmount : -moveAmount);
        else
            pixelPosition.y = TargetPixelY;

        if (reachedX && reachedY)
            currentPathIndex++;


        currentPosition.x = round(pixelPosition.x /40);
        currentPosition.y = round(pixelPosition.y /40);
        
    }


    // stop at target (left side for now)
    float dojoPixelX = dojopos.x * 40 + 40;  // Center of dojo cell
    float dojoPixelY = dojopos.y * 40 + 40;
    float dx = pixelPosition.x - dojoPixelX;
    float dy = pixelPosition.y - dojoPixelY;
    float distanceToDojo = std::sqrt(dx*dx + dy*dy);

    //Stop when within 80 pixels of dojo center(adjust this value)
    if(distanceToDojo <= 60.0f){
        cout << "reached dojo" << endl;
        isMoving = false;
        reachedDojo = true;
        isAttacking = true;
    }
}

//commented out for future use in recursive pathfinding algorithm
void Enemy::setPath(vector<GridPosition> enemypath) {
    path = new vector<GridPosition>( enemypath);
    pathLength = path->size();
}

bool Enemy::hasReachedDojo() const { 
    return reachedDojo; 
}

bool Enemy::getIsActive() const { 
    return isActive; 
}

GridPosition Enemy::getPosition() const { 
    return currentPosition; 
}

int Enemy::getType() const { 
    return type; 
}

int Enemy::getHealth() const { 
    return health; 
}

int Enemy::getMaxHealth() const { 
    return maxHealth; 
}

int Enemy::getDamage() const { 
    return damage; 
}

sf::Vector2f Enemy::getPixelPosition() const{ 
    return pixelPosition; 
}

// Genin implementation
Genin::Genin(GridPosition spawnPos , GridPosition dojo) : Enemy(0, 50, 5 , 30.0f, spawnPos , dojo) {}
Genin::~Genin() {}

// Chunin implementation
Chunin::Chunin(GridPosition spawnPos , GridPosition dojo) : Enemy(1, 100, 10, 25.0f, spawnPos , dojo) {}
Chunin::~Chunin() {}

// Jonin implementation
Jonin::Jonin(GridPosition spawnPos , GridPosition dojo) : Enemy(2, 150, 20, 20.0f, spawnPos , dojo) {}
Jonin::~Jonin() {}