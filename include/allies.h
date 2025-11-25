#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>
#include <vector>
#include "enemy.h"

#define MAX_ARROWS 20
#define MAX_QUEUE_SIZE 20

class EnemyPriorityQueue{
private:
    Enemy** heap;
    int heapSize;
    int capacity;

    bool hasHigherPriority(Enemy* a, Enemy* b){
        if(!a || !b) return false;
        
        if(a->getType() != b->getType()){
            return a->getType() > b->getType();
        }
        return a->getHealth() > b->getHealth();
    }

    void heapifyUp(int index){
        while(index > 0){
            int parent = (index - 1) / 2;
            if(hasHigherPriority(heap[index], heap[parent])){
                Enemy* temp = heap[index];
                heap[index] = heap[parent];
                heap[parent] = temp;
                index = parent;
            } 
            else{
                break;
            }
        }
    }

    void heapifyDown(int index){
        while(true){
            int largest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if(left < heapSize && hasHigherPriority(heap[left], heap[largest])){
                largest = left;
            }
            if(right < heapSize && hasHigherPriority(heap[right], heap[largest])){
                largest = right;
            }

            if(largest != index){
                Enemy* temp = heap[index];
                heap[index] = heap[largest];
                heap[largest] = temp;
                index = largest;
            } 
            else{
                break;
            }
        }
    }

public:
    EnemyPriorityQueue(){
        capacity = MAX_QUEUE_SIZE;
        heap = new Enemy*[capacity];
        heapSize = 0;
    }

    ~EnemyPriorityQueue(){
        delete[] heap;
    }

    void push(Enemy* enemy){
        if(heapSize < capacity){
            heap[heapSize] = enemy;
            heapifyUp(heapSize);
            heapSize++;
        }
    }

    void pop(){
        if(heapSize > 0){
            heap[0] = heap[heapSize - 1];
            heapSize--;
            if(heapSize > 0){
                heapifyDown(0);
            }
        }
    }

    Enemy* top(){
        if(heapSize > 0){
            return heap[0];
        }
        return nullptr;
    }

    bool empty() const{
        return heapSize == 0;
    }

    int size() const{
        return heapSize;
    }

    void clear(){
        heapSize = 0;
    }
};

class Ally{
protected:
    int type;
    int damage;
    int range;
    float attackSpeed;
    float attackCooldown;
    GridPosition position;
    sf::Vector2f pixelPos;
    bool isActive;

public:
    Ally(int type, int damage, int range, float attackSpeed, GridPosition pos);
    virtual ~Ally();
    
    virtual void update(float deltaTime, Enemy** enemies, int enemyCount, Ally** allies = nullptr, int allyCount = 0) = 0;
    virtual void attack(Enemy* target) = 0;
    bool isInRange(const GridPosition& targetPos) const;
    int getType() const;
    GridPosition getPosition() const;
    bool getIsActive() const;
    sf::Vector2f getPixelPos()const{return pixelPos;}
    void setPosition(GridPosition newPos);
    
};

class Samurai : public Ally {
private:
    int health;
    int maxHealth;
    Enemy* currentTarget;
    bool isMoving;
    float animationTimer;   
    int currentAnimFrame;   

        
public:
    Samurai(GridPosition pos);
    
    ~Samurai();
    
    void update(float deltaTime, Enemy** enemies, int enemyCount, Ally** allies, int allyCount) override;
    void attack(Enemy* target) override;
    void takeDamage(int damage);
    int getHealth() const;
    int getMaxHealth() const;
    void findClosestTarget(Enemy** enemies, int enemyCount);
    bool isrunning() const;
    void moveToTarget(float deltaTime, Ally** allies, int allyCount);
    bool wouldCollide(const sf::Vector2f& newPos, Ally** allies, int allyCount) const;
    bool isInAttackRange() const;
    int getAnimationFrame() const;
};

class ArcherTower : public Ally{
private:
    class Arrow* arrows[MAX_ARROWS];
    float arrowSpeed;
    int arrowCount;
    EnemyPriorityQueue enemyQueue;

public:
    ArcherTower(GridPosition pos);
    ~ArcherTower();
    
    void update(float deltaTime, Enemy** enemies, int enemyCount, Ally** allies, int allyCount) override;
    void attack(Enemy* target) override;
    void spawnArrow(Enemy* target);
    class Arrow** getArrows();
    int getArrowCount() const;

    void updateEnemyQueue(Enemy** enemies, int enemyCount);
    bool isInRange(const Enemy* enemy) const;
    void cleanupQueue();
};

class Arrow {
private:
    sf::Vector2f startPosition;
    sf::Vector2f currentPosition;
    sf::Vector2f targetPosition;
    Enemy* target;
    int damage;
    float speed;
    bool active;

public:
    Arrow(sf::Vector2f startPos, Enemy* target, int damage, float speed);
    ~Arrow();
    
    void update(float deltaTime);
    void checkHit();
    bool isActive() const;
    sf::Vector2f getStartPosition() const{ return startPosition; }
    sf::Vector2f getCurrentPosition() const{ return currentPosition; }
    sf::Vector2f getTargetPosition() const;
    Enemy* getTarget() const{ return target; }
};