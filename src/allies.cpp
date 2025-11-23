#include "allies.h"


Ally::Ally(int type, int damage, int range, float attackSpeed, GridPosition pos) 
    : type(type), damage(damage), range(range), attackSpeed(attackSpeed), position(pos), isActive(true), attackCooldown(0.0f)
    {
        pixelPos.x = pos.x * 40.0f + 20.0f; 
        pixelPos.y = pos.y * 40.0f + 20.0f;
    }

Ally::~Ally() {}

bool Ally::isInRange(const GridPosition& targetPos) const{
    int dx = std::abs(position.x - targetPos.x);
    int dy = std::abs(position.y - targetPos.y);
    return (dx <= range && dy <= range);
}

int Ally::getType() const { 
    return type; 
}

GridPosition Ally::getPosition() const{ 
    return position; 
}

bool Ally::getIsActive() const{ 
    return isActive; 
}

void Ally::setPosition(GridPosition newPos){ 
    position = newPos; 
}

// Samurai implementation
Samurai::Samurai(GridPosition pos) : Ally(0, 20, 1, 1.0f, pos), health(120), maxHealth(100),
currentTarget(nullptr), isMoving(false){}

Samurai::~Samurai() {}

bool Samurai::isrunning() const{
    return isMoving;
}

void Samurai::update(float deltaTime, Enemy** enemies, int enemyCount, Ally** allies, int allyCount){
    if(!isActive) return;
    
    attackCooldown -= deltaTime;
    
    // Static timers shared by ALL samurai
    static float sharedAnimTimer = 0.0f;
    static int sharedAttackFrame = 0;
    static int sharedRunFrame = 0;
    
    if(isInAttackRange()) {
        // Update shared attack animation
        sharedAnimTimer += deltaTime;
        if(sharedAnimTimer >= 0.15f) {
            sharedAttackFrame = (sharedAttackFrame + 1) % 4;
            sharedAnimTimer = 0.0f;
        }
        currentAnimFrame = sharedAttackFrame;
    } 
    else if(isMoving) {
        // Update shared run animation
        sharedAnimTimer += deltaTime;
        if(sharedAnimTimer >= 0.1f) {
            sharedRunFrame = (sharedRunFrame + 1) % 4;
            sharedAnimTimer = 0.0f;
        }
        currentAnimFrame = sharedRunFrame;
    } 
    else {
        
        currentAnimFrame = 0;
        sharedAnimTimer = 0.0f;
    }
    
    if(!currentTarget || !currentTarget->getIsActive()){
        findClosestTarget(enemies, enemyCount);
    }

    if(currentTarget && currentTarget->getIsActive()){
        if(isInAttackRange()){
            isMoving = false;
            if(attackCooldown <= 0.0f){
                attack(currentTarget);
                attackCooldown = 1.0f / attackSpeed;
            }
        } 
        else{
            isMoving = true;
            moveToTarget(deltaTime, allies, allyCount);
        }
    }
}

void Samurai::findClosestTarget(Enemy** enemies, int enemyCount){
    currentTarget = nullptr;
    float closestDistance = std::numeric_limits<float>::max(); //tis is the biggest float value
    
    for(int i = 0; i < enemyCount; i++){
        if(enemies[i] && enemies[i]->getIsActive()){
            sf::Vector2f enemyPos = enemies[i]->getPixelPosition();
            float dx = pixelPos.x - enemyPos.x;
            float dy = pixelPos.y - enemyPos.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            
            if(distance < closestDistance){
                closestDistance = distance;
                currentTarget = enemies[i];
            }
        }
    }
    
    if(currentTarget){
        isMoving = true;
    }
}

void Samurai::moveToTarget(float deltaTime, Ally** allies, int allyCount){
    if (!currentTarget || !isMoving) return;
    
    sf::Vector2f targetPos = currentTarget->getPixelPosition();
    sf::Vector2f direction = targetPos - pixelPos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    
    if(distance <= 55.0f){
        isMoving = false;
        return;
    }
    
    if(distance > 0){
        direction.x /= distance;
        direction.y /= distance;
    }
    
    float moveSpeed = 60.0f; 
    float moveAmount = moveSpeed * deltaTime;
    
    if(moveAmount > distance - 45.0f){
        moveAmount = distance - 45.0f;
    }
    
    sf::Vector2f newPosition = pixelPos;
    newPosition.x += direction.x * moveAmount;
    newPosition.y += direction.y * moveAmount;

    if(!currentTarget->hasReachedDojo()){
        if(!wouldCollide(newPosition, allies, allyCount)) pixelPos = newPosition;
    }
    else{
        pixelPos = newPosition;
    }
}

bool Samurai::wouldCollide(const sf::Vector2f& newPos, Ally** allies, int allyCount) const{
    for(int i = 0; i < allyCount; i++){
        if(allies[i] && allies[i] != this && allies[i]->getIsActive()){
            sf::Vector2f otherPos = allies[i]->getPixelPos();
            float dx = newPos.x - otherPos.x;
            float dy = newPos.y - otherPos.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            if(dist < 5.0f){
                return true;
            }
        }
    }
    return false;
}

bool Samurai::isInAttackRange() const{
    if(!currentTarget) return false;
    
    sf::Vector2f targetPos = currentTarget->getPixelPosition();
    float dx = pixelPos.x - targetPos.x;
    float dy = pixelPos.y - targetPos.y;
    float distance = std::sqrt(dx*dx + dy*dy);
    
    return distance <= 55.0f;
}

void Samurai::attack(Enemy* target) {
    if(target){
        target->takeDamage(damage);
    }
}

int Samurai::getAnimationFrame() const 
    { return currentAnimFrame; }

void Samurai::takeDamage(int damage){
    health -= damage;
    if(health <= 0){
        health = 0;
        isActive = false;
    }
}

int Samurai::getHealth() const{ 
    return health; 
}

int Samurai::getMaxHealth() const{ 
    return maxHealth; 
}

// ArcherTower implementation
ArcherTower::ArcherTower(GridPosition pos) 
    : Ally(1, 5, 5, 0.5f, pos), arrowSpeed(2.0f), arrowCount(0)
{
    for(int i = 0; i < MAX_ARROWS; i++) arrows[i] = nullptr;
}

ArcherTower::~ArcherTower(){
    for(int i = 0; i <MAX_ARROWS; i++) delete arrows[i];
}

bool ArcherTower::isInRange(const Enemy* enemy) const{
    if(!enemy || !enemy->getIsActive()) return false;
    
    GridPosition enemyPos = enemy->getPosition();
    int dx = std::abs(position.x - enemyPos.x);
    int dy = std::abs(position.y - enemyPos.y);
    
    // 4 block radius in all directions
    return (dx <= 4 && dy <= 4);
}

void ArcherTower::updateEnemyQueue(Enemy** enemies, int enemyCount){
    enemyQueue.clear();
    
    for(int i = 0; i < enemyCount; i++){
        if(enemies[i] && enemies[i]->getIsActive() && isInRange(enemies[i])){
            enemyQueue.push(enemies[i]);
        }
    }
}

//helper function to remove dead or out of range enemies from queue
void ArcherTower::cleanupQueue(){
    Enemy* validEnemies[MAX_QUEUE_SIZE];
    int validCount = 0;
    
    while(!enemyQueue.empty()){
        Enemy* enemy = enemyQueue.top();
        enemyQueue.pop();
        if(enemy && enemy->getIsActive() && isInRange(enemy)){
            validEnemies[validCount++] = enemy;
        }
    }
    
    for(int i = 0; i < validCount; i++){ //rebuild queue..
        enemyQueue.push(validEnemies[i]);
    }
}

void ArcherTower::update(float deltaTime, Enemy** enemies, int enemyCount, Ally** allies = nullptr, int allyCount =0){
    if(!isActive) return;
    
    attackCooldown -= deltaTime;
    
    // Update arrows
    for(int i = 0; i < MAX_ARROWS; i++){
        if(arrows[i] && arrows[i]->isActive()){
            arrows[i]->update(deltaTime);
        }
    }  
    
    updateEnemyQueue(enemies, enemyCount);
    
    
    cleanupQueue();
    //attack enemy through priority
    if(!enemyQueue.empty() && attackCooldown <= 0.0f){
        Enemy* target = enemyQueue.top();
        if(target && target->getIsActive() && isInRange(target)){
            attack(target);
            attackCooldown = 1.0f / attackSpeed;
        }else{
            enemyQueue.pop();
        }
    }
}
void ArcherTower::attack(Enemy* target){
    spawnArrow(target);
}
void ArcherTower::spawnArrow(Enemy* target){
    for(int i = 0; i < MAX_ARROWS; i++){
        if(!arrows[i] || !arrows[i]->isActive()){
            delete arrows[i];
            arrows[i] = new Arrow(pixelPos, target, damage, arrowSpeed);
            break;
        }
    }
}
Arrow** ArcherTower::getArrows(){ 
    return arrows; 
}

int ArcherTower::getArrowCount() const{ 
    return arrowCount; 
}

// Arrow implementation
Arrow::Arrow(sf::Vector2f startPos, Enemy* target, int damage, float speed) 
    : currentPosition(startPos), target(target), damage(damage), speed(speed), active(true) {}

Arrow::~Arrow() {}

sf::Vector2f Arrow::getTargetPosition() const {
    if(target && target->getIsActive()){
        return target->getPixelPosition();
    }
    return currentPosition;
}

void Arrow::update(float deltaTime){
    if (!active || !target || !target->getIsActive()) {
        active = false;
        return;
    }
    
    targetPosition = target->getPixelPosition();
    
    sf::Vector2f direction = targetPosition - currentPosition;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if(distance < 5.0f){ 
        checkHit();
        return;
    }
    
    //normalize direction for constant speed
    if(distance > 0){
        direction.x /= distance;
        direction.y /= distance;
    }
    
    float moveAmount = speed * deltaTime * 50.0f; 
    currentPosition.x += direction.x * moveAmount;
    currentPosition.y += direction.y * moveAmount;
    
    sf::Vector2f newDirection = targetPosition - currentPosition;
    float newDistance = std::sqrt(newDirection.x * newDirection.x + newDirection.y * newDirection.y);
    
    if(newDistance < 5.0f){
        checkHit();
    }
}

void Arrow::checkHit(){
    if (target && target->getIsActive()){
        target->takeDamage(damage);
    }
    active = false;
}

bool Arrow::isActive() const{ 
    return active; 
}