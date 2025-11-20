#include "game.h"
#include "map.h"
#include <iostream>

Game::Game() :
                currentMap(nullptr), playerDojo(nullptr), allies(nullptr), enemies(nullptr),
                currentWave(0), samuraiLeft(4), archerTowersLeft(16), gameRunning(false), gameWon(false), gameLost(false),
                maxAllies(50), maxEnemies(100), allyCount(0),spawnTimer(0.0f), spawnInterval(2.0f), enemiesToSpawn(20), enemiesSpawned(0), enemiesDefeated(0), enemyCount(0), windowheight(sf::VideoMode::getDesktopMode().size.y * 0.8f), windowwidth(sf::VideoMode::getDesktopMode().size.x * 0.8f),
                window(sf::VideoMode({static_cast<unsigned int>(sf::VideoMode::getDesktopMode().size.x * 0.8f),static_cast<unsigned int>(sf::VideoMode::getDesktopMode().size.y * 0.8f)}),"DojoDefender"), currentScreen(0), showTitleScreens(true)
               {
    allies = new Ally*[maxAllies];
    enemies = new Enemy*[maxEnemies];
    for(int i = 0; i < maxAllies; i++) allies[i] = nullptr;
    for(int i = 0; i < maxEnemies; i++) enemies[i] = nullptr;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int posX = (desktop.getDesktopMode().size.x - windowwidth) / 2;
    int posY = (desktop.getDesktopMode().size.y - windowheight) / 2.5;
    window.setPosition({posX,posY});
    enemyspawnpoint = GridPosition(0,0);
}

Game::~Game() {
    cleanup();
}

void Game::loadTitleScreens(){
    if(!titleScreen.loadFromFile("images/Cover.png")) {
        cout << "Failed to load title screen 1" << endl;
    }
    if(!howToPlayScreen.loadFromFile("images/Instructions1.png")) {
        cout << "Failed to load title screen 2" << endl;
    }
}

void Game::initialize() {
    loadTitleScreens();

    // Initialize all
    if(!BlackRunning.loadFromFile("images/Blackninjarun.png")){
        cout<<"Failed to load black ninja running ";
    }
    if(!BlackAttack.loadFromFile("images/BlackAttack.png")){
        cout<<"Failed to load black attack";
    }
    if(!WhiteAttack.loadFromFile("images/WhiteAttack.png")){
        cout<<"Failed to load White Attack";
    }
    if(!YellowAttack.loadFromFile("images/YellowAttack.png")){
        cout<<"Failed to load Yellow attack";
    }
    if(!WhiteIdle.loadFromFile("images/WhiteIdle.png")){
        cout<<"Failed to load white idle";
    }
    if(!BlackIdle.loadFromFile("images/BlackIdle.png")){
        cout<<"Failed to load Black idle"<<endl;
    }
    if(!YellowIdle.loadFromFile("images/yellowidle.png")){
        cout<<"Failed to load yellow idle";
    }
    if(!Archertowertex.loadFromFile("images/ArcherTower.png")){
        cout<<"Failed to load Archer tower";
    }
    if (!font.openFromFile("assets/font.ttf")) {
        std::cout << "Failed to load font, using default" << std::endl;
    }
    if(!Yellowrunning.loadFromFile("images/YellowninjaRun.png")){
        cout<<"Failed to load yellow ninja running"<<endl;
    }
    if(!Whiterunning.loadFromFile("images/WhiteRun.png")){
        cout<<"Failed to load white run";
    }
    if(!SamuraiStanding.loadFromFile("images/SamuraiStanding.png")){
        cout<<"SamuraiStanding failed to load";
    }
    if(!SamuraiAttacking.loadFromFile("images/SamuraiAttacking.png")){
        cout<<"Failed to load samurai attack";
    }
    if(!SamuraiRunning.loadFromFile("images/SamuraiRunning.png")){
        cout<<"Failed to load samurai running";
    }
    if(!Arrowtexture.loadFromFile("images/Arrow.png")){
        cout<<"Failed to load arrow.png";
    }
    if(!Dojotexture.loadFromFile("images/Dojo.png")){
        cout <<"Failed to load dojo.png";
    }
    if (!boundarytree.loadFromFile("images/Boundary.png")) 
        {cout << "Failed to load tree.png\n";}
    if (!obstacletree.loadFromFile("images/Obstacle.png")) 
        {cout << "Failed to load tree.png\n";}
    if(!grass.loadFromFile("images/Grass.png")){
        cout << "Failed to load grass.png\n";
    }
    if(!Spawn.loadFromFile("images/Enemyspawn.png"));{
        cout<<"Failed to load enemyspawn";
    }
    if(!Soil.loadFromFile("images/Soil.png")){
        cout<<"Failed to load soil";
    }

    currentMap = new Map();
    playerDojo = new Dojo(currentMap->getDojoPosition(), 100);
    enemyspawnpoint = currentMap->getenemyspawn();

    initializeEnemyStack();

    gameRunning = true;
}

void Game::run() {
    sf::Clock clock;
    float deltaTime = 0.0f;
    window.setVerticalSyncEnabled(true);
    
    while(gameRunning && window.isOpen()) {
        deltaTime = clock.restart().asSeconds();
        if(showTitleScreens){
            handleTitleScreenEvents();
            renderTitleScreen();
        }
        else{
            handleEvents();
            if(!gameLost && !gameWon){
                update(deltaTime);
            }
            render();
            checkGameOver();
        }
    }
}

void Game::handleTitleScreenEvents(){
    while(auto eventOpt = window.pollEvent()){
        const sf::Event& event = *eventOpt;

        if(event.is<sf::Event::Closed>()){
            gameRunning = false;
            window.close();
            return;
        }

        else if(const auto* key = event.getIf<sf::Event::KeyPressed>()){
            if (key->scancode == sf::Keyboard::Scancode::Enter) {
                if(currentScreen == 0){
                    currentScreen = 1;
                } 
                else if(currentScreen == 1){
                    showTitleScreens = false;
                    currentScreen = 2;
                }
            }
            else if(key->scancode == sf::Keyboard::Scancode::Escape){
                gameRunning = false;
                window.close();
            }
        }
    }
}

void Game::renderTitleScreen(){
    window.clear(sf::Color::Black);
    
    sf::Sprite titleSprite(titleScreen);
    if(currentScreen == 0){
        titleSprite.setTexture(titleScreen);

    } 
    else{
        titleSprite.setTexture(howToPlayScreen);
    }
    
    sf::Vector2u textureSize = titleScreen.getSize();
    sf::Vector2f scale(static_cast<float>(windowwidth) / textureSize.x, static_cast<float>(windowheight) / textureSize.y);
    titleSprite.setScale(scale);
    
    window.draw(titleSprite);
    
    window.display();
}

void Game::handleEvents(){
    while (auto eventOpt = window.pollEvent()) {
        const sf::Event& event = *eventOpt;

        if(event.is<sf::Event::Closed>() || !window.isOpen()){
            gameRunning = false;
            window.close();
        }

        else if(const auto* key = event.getIf<sf::Event::KeyPressed>()){
            if(key->scancode == sf::Keyboard::Scancode::Escape) {
                gameRunning = false;
                window.close();
            }
        }

        else if(const auto* mousekey = event.getIf<sf::Event::MouseButtonPressed>()){
            if (mousekey->button == sf::Mouse::Button::Left) {
                // Left click - Archer Tower
                int gridX = mousekey->position.x / 40;
                int gridY = mousekey->position.y / 40;
                if (canPlaceAlly(gridX, gridY)) {
                    placeAlly(1, gridX, gridY); // Type 1 = Archer Tower
                    cout << "Placed Archer Tower" << endl;
                } else {
                    cout << "Cannot place Archer Tower here" << endl;
                }
            }
            else if(mousekey->button == sf::Mouse::Button::Right){
                int gridX = mousekey->position.x / 40;
                int gridY = mousekey->position.y / 40;
                if(canPlaceAlly(gridX, gridY)){
                    placeAlly(0, gridX, gridY); // Type 0 = Samurai
                    cout << "Placed Samurai" << endl;
                } else {
                    cout << "Cannot place Samurai here" << endl;
                }
            }
        }

        else if(const auto* resized = event.getIf<sf::Event::Resized>()){
            std::cout << "Window resized to " 
                      << resized->size.x << "x" << resized->size.y << "\n";
        }
    }
}


void Game::update(float deltaTime) {
    if(allyCount != 0){spawnFromStack(deltaTime);}

    // Update enemies
    for(int i = 0; i < enemyCount; i++) {
        if(enemies[i] && enemies[i]->getIsActive()) {
            enemies[i]->update(deltaTime, currentMap , allies , playerDojo, allyCount);
        }
    }
    
    // Update allies
    for(int i = 0; i < allyCount; i++) {
        if(allies[i] && allies[i]->getIsActive()) {
            allies[i]->update(deltaTime, enemies, enemyCount, allies, allyCount);
        }
    }
}

void Game::render() {
    window.clear(sf::Color(50, 50, 50));
    
    renderMap();
    renderDojo();
    renderEnemyspawn();
    
    // Render allies
    for(int i = 0; i < allyCount; i++) {
        if(allies[i] && allies[i]->getIsActive()) {
            renderAlly(*allies[i]);
        }
    }
    
    // Render enemies
    for(int i = 0; i < enemyCount; i++) {
        if(enemies[i] && enemies[i]->getIsActive()) {
            renderEnemy(*enemies[i]);
        }
    }

    renderArrows();
    
    renderUI();
    window.display();
}


void Game::renderDojo() {
    if (!playerDojo) return;

    float cellWidth  = static_cast<float>(windowwidth) / currentMap->getWidth();
    float cellHeight = static_cast<float>(windowheight) / currentMap->getHeight();

    sf::Sprite dojosprite(Dojotexture);
    GridPosition pos = playerDojo->getPosition();
    sf::Vector2f dojopos(pos.x * 35 , pos.y *40 );
    sf::Vector2f dojoscale((cellWidth * 3) / Dojotexture.getSize().x, (cellHeight * 3) / Dojotexture.getSize().y);
    dojosprite.setPosition(dojopos);
    dojosprite.setScale(dojoscale);
    window.draw(dojosprite);
    // Health bar
    float healthPercent = playerDojo->getHealthPercentage();
    sf::RectangleShape healthBar(sf::Vector2f(60 * healthPercent, 5));
    sf::Vector2f healthBarpos(pos.x * 52 , pos.y*40 );
    healthBar.setPosition(healthBarpos);
    healthBar.setFillColor(sf::Color::Green);
    window.draw(healthBar);
}


void Game::renderMap() {
    if (!currentMap) return;

    float cellWidth  = static_cast<float>(windowwidth) / currentMap->getWidth();
    float cellHeight = static_cast<float>(windowheight) / currentMap->getHeight();
    
    sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));
    cell.setOutlineThickness(1);
    cell.setOutlineColor(sf::Color(139, 69, 19));
    
    int width = currentMap->getWidth();
    int height = currentMap->getHeight();

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            sf::Vector2f position(x * cellWidth, y * cellHeight);
            cell.setPosition(position);
            
            //cell background color
            if(currentMap->getCellType(x,y) == 1) {
                cell.setFillColor(sf::Color(255, 183, 197)); // Cherry blossom area
            } 
            else if(currentMap->getCellType(x,y) == 2) {
                cell.setFillColor(sf::Color(184, 134, 72)); // Path/ground
            }
            else if(currentMap->getCellType(x,y) == 3){
                cell.setFillColor(sf::Color::White);
            }
            else if(currentMap->getCellType(x,y) == 4){
                cell.setFillColor(sf::Color::Magenta);
            }
            else if(currentMap->getCellType(x,y) == 8){
                cell.setFillColor(sf::Color::Black);
            }
            else if(currentMap->getCellType(x,y) == 9){
                cell.setFillColor(sf::Color::Cyan);
            }
            else{
                cell.setFillColor(sf::Color(126, 200, 80)); // Default grass
            }
            window.draw(cell);

           

            // Draw grass everywhere 
            if (grass.getSize().x > 0 && grass.getSize().y > 0 ) {
                sf::Sprite groundgrass(grass);
                groundgrass.setPosition(position);
                sf::Vector2f GrassScale((cellWidth) / grass.getSize().x, (cellHeight) / grass.getSize().y);
                groundgrass.setScale(GrassScale);
                window.draw(groundgrass);
            }

            // drawing obstacle trees where ever 2 is present
            if (obstacletree.getSize().x > 0 && obstacletree.getSize().y > 0 && currentMap->getCellType(x,y) == 2) {
                sf::Sprite Obstacle(obstacletree);
                sf::Vector2f treeposition((x * cellWidth) -15, (y * cellHeight) -20);
                Obstacle.setPosition(treeposition);
                sf::Vector2f Obstacletreescale((cellWidth * 1.7) / obstacletree.getSize().x, (cellHeight * 1.7) / obstacletree.getSize().y);
                Obstacle.setScale(Obstacletreescale);
                window.draw(Obstacle);
            }

            // drawing boundary trees at 1s
            if (boundarytree.getSize().x > 0 && boundarytree.getSize().y > 0 && currentMap->getCellType(x,y) == 1) {
                sf::Sprite Boundary(boundarytree);
                sf::Vector2f boundaryPosition((x * cellWidth) -10, (y * cellHeight) -18);
                Boundary.setPosition(boundaryPosition);
                sf::Vector2f Boundarytreescale((cellWidth * 1.5) / boundarytree.getSize().x, (cellHeight * 1.5) / boundarytree.getSize().y);
                Boundary.setScale(Boundarytreescale);
                window.draw(Boundary);
            }


            
        }
    }
}

    


void Game::renderAlly(const Ally& ally) {
    float cellWidth  = static_cast<float>(windowwidth) / currentMap->getWidth();
    float cellHeight = static_cast<float>(windowheight) / currentMap->getHeight();
    
        sf::Vector2f allyPos = ally.getPixelPos();
        allyPos.y -= 60;
        allyPos.x -= 30;    
        if(ally.getType() == 0){  // Samurai
    
        Samurai* warrior = const_cast<Samurai*>(dynamic_cast<const Samurai*>(&ally));
        
        if(!warrior) return;
        
    
        if(warrior->isInAttackRange()) {
            // When attacking 
            sf::Sprite Samuraisprite(SamuraiAttacking);
            
            int columns = 4;
            int rows = 1;
            
            
            static float animationTime = 0.0f;
            static int currentFrame = 0;
            
            animationTime += 0.02f;
            
            if(animationTime >= 0.3f) {
                currentFrame = (currentFrame + 1) % 4;
                animationTime = 0.0f;  
            }
            
            int col = currentFrame % columns;
            int row = currentFrame / columns;
            
            int spriteWidth = static_cast<int>(SamuraiAttacking.getSize().x) / columns;
            int spriteHeight = static_cast<int>(SamuraiAttacking.getSize().y) / rows;
            
            Samuraisprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight ),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Samuraisprite.setPosition(allyPos);
            
            float scale = (cellWidth * 3) / spriteWidth;
            Samuraisprite.setScale(sf::Vector2f(scale, scale));
            
            window.draw(Samuraisprite); 
            
        } else if(warrior->isrunning()) {
            // if the samurai is running
            sf::Sprite Samuraisprite(SamuraiRunning);
            
            int columns = 6;
            int rows = 1;

            static float animationTime = 0.0f;
            static int currentFrame = 0;
            
            animationTime += 0.02f;
            
            if(animationTime >= 0.15f) {
                currentFrame = (currentFrame + 1) % 4;
                animationTime = 0.0f; 
            }
            
            int col = currentFrame % columns;
            int row = currentFrame / columns;
            
            int spriteWidth = static_cast<int>(SamuraiRunning.getSize().x) / columns;
            int spriteHeight = static_cast<int>(SamuraiRunning.getSize().y) / rows;
            
            Samuraisprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Samuraisprite.setPosition(allyPos);
            
            float scale = (cellWidth * 3) / spriteWidth;
            Samuraisprite.setScale(sf::Vector2f(scale, scale));
            window.draw(Samuraisprite);
            
        } else {
            // STANDING/IDLE
            sf::Sprite SamuraiSprite(SamuraiStanding);
            SamuraiSprite.setPosition(allyPos);
            float scale = (cellWidth * 3) / SamuraiStanding.getSize().x;
            SamuraiSprite.setScale(sf::Vector2f(scale, scale));
            window.draw(SamuraiSprite);
        }
        
    } else {  // Archer Tower
        sf::Sprite TowerSprite(Archertowertex);
        TowerSprite.setPosition(allyPos);
        float scale = (cellWidth * 1.5) / Archertowertex.getSize().x;
        TowerSprite.setScale(sf::Vector2f(scale, scale));
        window.draw(TowerSprite);
    }
}

void Game::renderEnemy(const Enemy& enemy) {

    if(enemy.getType() == 0 || enemy.getType() == 2) {  // White or Black
        cout << "Type " << enemy.getType() << " frame: " << enemy.getAnimationFrame() 
             << " moving: " << enemy.getismoving() << " attacking: " << enemy.getisAttacking() << endl;
    }

    float cellWidth  = static_cast<float>(windowwidth) / currentMap->getWidth();
    float cellHeight = static_cast<float>(windowheight) / currentMap->getHeight();
    
    sf::Vector2f enemyPos = enemy.getPixelPosition();
    enemyPos.x += 40;
    if(enemy.getType() == 0){  // Genin - White ninja
        sf::Sprite Enemysprite(Whiterunning);
        int columns = 5;
        int rows = 1;
        int currentFrame = enemy.getAnimationFrame();
        int col = currentFrame % columns;
        int row = currentFrame / columns;
        if(enemy.getismoving() && !enemy.getisAttacking()){
            
            int spriteWidth = static_cast<int>(Whiterunning.getSize().x) / columns;
            int spriteHeight = static_cast<int>(Whiterunning.getSize().y) / rows;
            
            Enemysprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Enemysprite.setPosition(enemyPos);
            sf::Vector2f origin(spriteWidth / 2.0f, spriteHeight / 2.0f);
            Enemysprite.setOrigin(origin);
            float scale = (cellWidth * 2.5) / spriteWidth;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
        }
        else if(enemy.getisAttacking() && !enemy.getismoving() ){
            Enemysprite.setTexture(WhiteAttack);
            int spriteWidth = static_cast<int>(WhiteAttack.getSize().x) / columns;
            int spriteHeight = static_cast<int>(WhiteAttack.getSize().y) / rows;
            
            Enemysprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Enemysprite.setPosition(enemyPos);
            sf::Vector2f origin(spriteWidth / 2.0f, spriteHeight / 2.0f);
        
            Enemysprite.setOrigin(origin);
            float scale = (cellWidth * 2.5) / spriteWidth;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
        }
        
        else{
            // NOT MOVING - use idle sprite
            Enemysprite.setTexture(WhiteIdle);
            Enemysprite.setPosition(enemyPos);
            float scale = (cellWidth * 3) / WhiteIdle.getSize().x;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
            sf::Vector2f origin(WhiteIdle.getSize().x / 2.0f, WhiteIdle.getSize().y / 2.0f);
            Enemysprite.setOrigin(origin);
        }
        window.draw(Enemysprite);  // IMPORTANT: Draw outside if/else
        
    } else if(enemy.getType() == 1) {  // Chunin - Yellow ninja
        sf::Sprite Enemysprite(Yellowrunning);
        int columns = 5;
        int rows = 1;
        int currentFrame = enemy.getAnimationFrame();
        int col = currentFrame % columns;
        int row = currentFrame / columns;
        if(enemy.getisAttacking() && !enemy.getismoving()){
            Enemysprite.setTexture(YellowAttack);
            int spriteWidth = static_cast<int>(YellowAttack.getSize().x) / columns;
            int spriteHeight = static_cast<int>(YellowAttack.getSize().y) / rows;
            
            Enemysprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Enemysprite.setPosition(enemyPos);
            float scale = (cellWidth * 2.6) / spriteWidth;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
            sf::Vector2f origin(spriteWidth / 2.0f, spriteHeight / 2.0f);
            Enemysprite.setOrigin(origin);
        }
        else if(enemy.getismoving() && !enemy.getisAttacking()){
            int spriteWidth = static_cast<int>(Yellowrunning.getSize().x) / columns;
            int spriteHeight = static_cast<int>(Yellowrunning.getSize().y) / rows;
            
            Enemysprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Enemysprite.setPosition(enemyPos);
            float scale = (cellWidth * 2.6) / spriteWidth;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
            sf::Vector2f origin(spriteWidth / 2.0f, spriteHeight / 2.0f);
            Enemysprite.setOrigin(origin);
        }
        
        else{
            // NOT MOVING - use idle sprite
            Enemysprite.setTexture(YellowIdle);
            Enemysprite.setPosition(enemyPos);
            float scale = (cellWidth * 1.6) / YellowIdle.getSize().x;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
            sf::Vector2f origin(YellowIdle.getSize().x / 2.0f, YellowIdle.getSize().y / 2.0f);
            Enemysprite.setOrigin(origin);
        }
        window.draw(Enemysprite);
        
    } else {  // Jonin - Black ninja
        sf::Sprite Enemysprite(BlackRunning);
            int columns = 5;
            int rows = 1;
            int currentFrame = enemy.getAnimationFrame();
            int col = currentFrame % columns;
            int row = currentFrame / columns;
        if(enemy.getismoving() && !enemy.getisAttacking()){
            
            int spriteWidth = static_cast<int>(BlackRunning.getSize().x) / columns;
            int spriteHeight = static_cast<int>(BlackRunning.getSize().y) / rows;
            
            Enemysprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Enemysprite.setPosition(enemyPos);
            float scale = (cellWidth * 2.5) / spriteWidth;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
            sf::Vector2f origin(spriteWidth / 2.0f, spriteHeight / 2.0f);
            Enemysprite.setOrigin(origin);
        }
        else if(enemy.getisAttacking() && enemy.getismoving()){
            Enemysprite.setTexture(BlackAttack);
            int spriteWidth = static_cast<int>(BlackAttack.getSize().x) / columns;
            int spriteHeight = static_cast<int>(BlackAttack.getSize().y) / rows;
            
            Enemysprite.setTextureRect(sf::IntRect(
                sf::Vector2i(col * spriteWidth, row * spriteHeight),
                sf::Vector2i(spriteWidth, spriteHeight)
            ));
            
            Enemysprite.setPosition(enemyPos);
            float scale = (cellWidth * 2.6) / spriteWidth;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
            sf::Vector2f origin(spriteWidth / 2.0f, spriteHeight / 2.0f);
            Enemysprite.setOrigin(origin);
        }
        
        else{
            // NOT MOVING - use idle sprite
            Enemysprite.setTexture(BlackIdle);
            Enemysprite.setPosition(enemyPos);
            float scale = (cellWidth * 2.8) / BlackIdle.getSize().x;
            Enemysprite.setScale(sf::Vector2f(-scale, scale));
            sf::Vector2f origin(BlackIdle.getSize().x / 2.0f, BlackIdle.getSize().y / 2.0f);
            Enemysprite.setOrigin(origin);
        }
        window.draw(Enemysprite);
    }
}

void Game::renderEnemyspawn() {
    
    float cellWidth  = static_cast<float>(windowwidth) / currentMap->getWidth();
    float cellHeight = static_cast<float>(windowheight) / currentMap->getHeight();
    sf::Sprite spawnsprite(Spawn);

    sf::RectangleShape shape(sf::Vector2f(60, 60));

    GridPosition pos = enemyspawnpoint;
    sf::Vector2f spawnpos(pos.x *39.5 , pos.y *35);
    shape.setPosition(spawnpos);
    shape.setFillColor(sf::Color::Magenta);
    sf::Vector2f spawnscale((cellWidth * 2.5) / Spawn.getSize().x, (cellHeight * 2.5) / Spawn.getSize().y);
    spawnsprite.setPosition(spawnpos);
    spawnsprite.setScale(spawnscale);
    
    window.draw(spawnsprite);
}


void Game::renderUI(){
    sf::RectangleShape resourceOverlay(sf::Vector2f(310.0f, 120.0f));
    resourceOverlay.setPosition(sf::Vector2f(15.0f, 20.0f));
    resourceOverlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(resourceOverlay);

    sf::Text text(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);

    text.setString("Samurai remaining: " + std::to_string(samuraiLeft));
    text.setPosition(sf::Vector2f(20.0f, 15.0f));
    window.draw(text);

    text.setString("Archer Towers remaining: " + std::to_string(archerTowersLeft));
    text.setPosition(sf::Vector2f(20.0f, 55.0f));
    window.draw(text);

    text.setString("Dojo Health: " + std::to_string(playerDojo->getHealth()));
    text.setPosition(sf::Vector2f(20.0f, 95.0f));
    window.draw(text);
 
    if(gameLost){
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowwidth), static_cast<float>(windowheight)));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        
        sf::Text gameOverText(font);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(80);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setStyle(sf::Text::Bold);

        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
        gameOverText.setPosition(sf::Vector2f(windowwidth / 2.0f, windowheight / 2.0f - 50));
        window.draw(gameOverText);

        sf::Text subtitleText(font);
        subtitleText.setString("Your Dojo Has Been Destroyed!");
        subtitleText.setCharacterSize(30);
        subtitleText.setFillColor(sf::Color::White);
        
        sf::FloatRect subtitleBounds = subtitleText.getLocalBounds();
        subtitleText.setOrigin(sf::Vector2f(subtitleBounds.size.x / 2.0f, subtitleBounds.size.y / 2.0f));
        subtitleText.setPosition(sf::Vector2f(windowwidth / 2.0f, windowheight / 2.0f + 50));
        window.draw(subtitleText);

        sf::Text instructionText(font);
        instructionText.setString("Press ESC to exit");
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(sf::Color(200, 200, 200));
        
        sf::FloatRect instructBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(sf::Vector2f(instructBounds.size.x / 2.0f, instructBounds.size.y / 2.0f));
        instructionText.setPosition(sf::Vector2f(windowwidth / 2.0f, windowheight / 2.0f + 120));
        window.draw(instructionText);
    }

    if(gameWon){
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(windowwidth), static_cast<float>(windowheight)));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        
        sf::Text victoryText(font);
        victoryText.setString("VICTORY");
        victoryText.setCharacterSize(80);
        victoryText.setFillColor(sf::Color::Green);
        victoryText.setStyle(sf::Text::Bold);

        sf::FloatRect textBounds = victoryText.getLocalBounds();
        victoryText.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
        victoryText.setPosition(sf::Vector2f(windowwidth / 2.0f, windowheight / 2.0f - 50));
        window.draw(victoryText);

        sf::Text subtitleText(font);
        subtitleText.setString("You successfully defended the Dojo!");
        subtitleText.setCharacterSize(30);
        subtitleText.setFillColor(sf::Color::White);
        
        sf::FloatRect subtitleBounds = subtitleText.getLocalBounds();
        subtitleText.setOrigin(sf::Vector2f(subtitleBounds.size.x / 2.0f, subtitleBounds.size.y / 2.0f));
        subtitleText.setPosition(sf::Vector2f(windowwidth / 2.0f, windowheight / 2.0f + 50));
        window.draw(subtitleText);

        sf::Text instructionText(font);
        instructionText.setString("Press ESC to exit");
        instructionText.setCharacterSize(20);
        instructionText.setFillColor(sf::Color(200, 200, 200));
        
        sf::FloatRect instructBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(sf::Vector2f(instructBounds.size.x / 2.0f, instructBounds.size.y / 2.0f));
        instructionText.setPosition(sf::Vector2f(windowwidth / 2.0f, windowheight / 2.0f + 120));
        window.draw(instructionText);
    }
}

void Game::renderArrows(){
    float cellWidth  = static_cast<float>(windowwidth) / currentMap->getWidth();
    float cellHeight = static_cast<float>(windowheight) / currentMap->getHeight();


    for(int i = 0; i < allyCount; i++){
        if(allies[i] && allies[i]->getIsActive() && allies[i]->getType() == 1){
            ArcherTower* tower = dynamic_cast<ArcherTower*>(allies[i]);
            if(tower){
                Arrow** arrows = tower->getArrows();
                
        
                for(int j = 0; j < MAX_ARROWS; j++){
                    if(arrows[j] && arrows[j]->isActive()){
            
                        sf::Vector2f currentPixel = arrows[j]->getCurrentPosition();
                        sf::Vector2f targetPixel = arrows[j]->getTargetPosition();
                        
                        sf::Vector2f direction = targetPixel - currentPixel;
                        sf::Sprite ArrowSprite(Arrowtexture);
                        sf::RectangleShape arrowRect(sf::Vector2f(30.0f, 2.0f));
                        arrowRect.setPosition(currentPixel);
                        ArrowSprite.setPosition(currentPixel);
                        arrowRect.setFillColor(sf::Color::White);  
                        float angleRadians = std::atan2(direction.y, direction.x);
                        sf::Angle rotationAngle = sf::radians(angleRadians);
                        sf::Vector2f Arrowscale((cellWidth)/Arrowtexture.getSize().x , (cellHeight)/Arrowtexture.getSize().y);
                        ArrowSprite.scale(Arrowscale);
                        ArrowSprite.setRotation(rotationAngle/3);  
                        arrowRect.setRotation(rotationAngle);
                            
                        window.draw(ArrowSprite);
                        
                    }
                }
            }
        }
    }
}



GridPosition Game::getenemyspawnpoint() const{
    return enemyspawnpoint;
}

void Game::initializeEnemyStack(){

    GridPosition tempspawn = enemyspawnpoint;
    enemyStack.push(new Jonin(tempspawn , playerDojo->getPosition()));
    enemyStack.push(new Genin(tempspawn, playerDojo->getPosition()));
    for (int i = 0; i < 3; i++) {
        enemyStack.push(new Jonin(tempspawn  , playerDojo->getPosition()));
        enemyStack.push(new Chunin(tempspawn , playerDojo->getPosition()));
        enemyStack.push(new Chunin(tempspawn , playerDojo->getPosition()));
        enemyStack.push(new Genin(tempspawn  , playerDojo->getPosition()));
        enemyStack.push(new Genin(tempspawn  , playerDojo->getPosition()));
        enemyStack.push(new Genin(tempspawn  , playerDojo->getPosition()));
    }
    cout << "Enemy stack initialized with " << enemyStack.size() << " enemies" << std::endl;
}

void Game::spawnFromStack(float deltaTime){
    if (enemiesSpawned >= enemiesToSpawn) return;
    
    spawnTimer += deltaTime;
    
    if (spawnTimer >= spawnInterval && !enemyStack.empty()) {
        if(enemyCount < maxEnemies){
            Enemy* newEnemy = enemyStack.top();
            enemyStack.pop();
            newEnemy->setPath(currentMap->getoptimumpath(allies, allyCount)) ;
            enemies[enemyCount] = newEnemy;
            enemyCount++;
            enemiesSpawned++;
            cout << "Spawned enemy from stack. Type: " << newEnemy->getType() 
                      << " Remaining in stack: " << enemyStack.size() << std::endl;
        }
        
        spawnTimer = 0.0f;
    }
}

void Game::checkGameOver(){
    if(playerDojo->isDojoDestroyed()){
        gameLost = true;
    }
    bool noMoreEnemiesToSpawn = (enemiesSpawned >= enemiesToSpawn) && enemyStack.empty();
    bool noActiveEnemies = true;
    
    for(int i = 0; i < enemyCount; i++){
        if(enemies[i] && enemies[i]->getIsActive()){
            noActiveEnemies = false;
            break;
        }
    }
    
    if(noMoreEnemiesToSpawn && noActiveEnemies){
        gameWon = true;
    }
}

void Game::placeAlly(int type, int gridX, int gridY) {
    if(allyCount >= maxAllies) return;
    
    if(type == 0){ // Samurai
        if(samuraiLeft > 0) {
            allies[allyCount] = new Samurai(GridPosition(gridX, gridY));
            samuraiLeft--;
        }
    } 
    else{ // Archer Tower
        if(archerTowersLeft > 0){
            allies[allyCount] = new ArcherTower(GridPosition(gridX, gridY));
            archerTowersLeft--;
        }

    }
    allyCount++;
}

void Game::removeAlly(int gridX, int gridY) {
    for (int i = 0; i < allyCount; i++) {
        if (allies[i] && allies[i]->getIsActive()) {
            GridPosition pos = allies[i]->getPosition();
            if (pos.x == gridX && pos.y == gridY) {
                delete allies[i];
                allies[i] = nullptr;
                // Shift array to remove gap
                for (int j = i; j < allyCount - 1; j++) {
                    allies[j] = allies[j + 1];
                }
                allies[allyCount - 1] = nullptr;
                allyCount--;
                break;
            }
        }
    }
}

bool Game::canPlaceAlly(int gridX, int gridY) {
    if (!currentMap) {
        std::cout << "currentMap is null!\n";
        return false;
    }
    if (!currentMap->isValidPosition(gridX, gridY)) {
        std::cout << "Invalid position: " << gridX << "," << gridY << "\n";
        return false;
    }
    if (currentMap->getCellType(gridX,gridY) == 2 || currentMap->getCellType(gridX,gridY) == 1) {
        std::cout << "Cell not free: " << gridX << "," << gridY << "\n";
        return false;
    }

    for (int i = 0; i < allyCount; i++) {
        if (allies[i] && allies[i]->getIsActive()) {
            GridPosition pos = allies[i]->getPosition();
            if (pos.x == gridX && pos.y == gridY) {
                std::cout << "Cell already has ally: " << gridX << "," << gridY << "\n";
                return false;
            }
        }
    }

    std::cout << "Can place ally at " << gridX << "," << gridY << "\n";
    return true;
}


void Game::cleanup() {
    delete currentMap;
    delete playerDojo;
    if(allies) {
        for(int i = 0; i < maxAllies; i++) delete allies[i];
        delete[] allies;
    }
    if(enemies) {
        for(int i = 0; i < maxEnemies; i++) delete enemies[i];
        delete[] enemies;
    }
}