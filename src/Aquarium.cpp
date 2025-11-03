#include "Aquarium.h"
#include <cstdlib>


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::JellyFish:
            return "JellyFish";
        case AquariumCreatureType::FastFish:
            return "FastFish";
        case AquariumCreatureType::PowerUp:
            return "PowerUp";
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, false, sprite) {}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    this->bounce(nullptr);
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    if(m_speedBoostFrames > 0){
        --m_speedBoostFrames;
        if(m_speedBoostFrames == 0){
            m_speed = m_base_speed;
            m_isBoosted = false;
        }
    }
    this->move();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, false, sprite) {
    m_dx = (rand() % 3 - 1); // -1, 0, or 1
    m_dy = (rand() % 3 - 1); // -1, 0, or 1
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce(nullptr);
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce(nullptr);
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}

JellyFish::JellyFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, std::max(1, speed/2), sprite){
    m_creatureType = AquariumCreatureType::JellyFish;
    setCollisionRadius(28);
    m_value = 2;
}

void JellyFish::move(){
    t += 0.05f;
    m_x += (m_dx == 0 ? 1 : m_dx) * (m_speed * 0.4f);
    m_y += std::sin(t) * 1.8f;
    bounce(nullptr);
}

void JellyFish::draw() const{
    m_sprite->draw(m_x, m_y);
}

FastFish::FastFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite )
:NPCreature(x, y, std::max(speed, 6), sprite){
    m_creatureType = AquariumCreatureType::FastFish;
    setCollisionRadius(24);
    m_value = 3;
}

void FastFish::move(){
    ++step;
    float zig = (step % 30 < 15) ? 1.0f : -1.0f;
    m_x += (m_dx == 0 ? 1 : m_dx) * (m_speed * 1.2f);
    m_y += zig * 0.9f;
    bounce(nullptr);
}

void FastFish::draw() const{
    m_sprite->draw(m_x, m_y);
}


// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_jelly_fish = std::make_shared<GameSprite>("jelly-fish.png", 80, 80);
    this->m_fast_fish = std::make_shared<GameSprite>("fast-fish.png", 70, 70);
    this->m_powerup = std::make_shared<GameSprite>("power-up.png", 40, 40);


}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);
        case AquariumCreatureType::JellyFish:
            return std::make_shared<GameSprite>(*this->m_jelly_fish);
        case AquariumCreatureType::FastFish:
            return std::make_shared<GameSprite>(*this->m_fast_fish);
        case AquariumCreatureType::PowerUp:
            return std::make_shared<GameSprite>(*this->m_powerup);
        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    for (auto& creature : m_creatures) {
        creature->move();
    }
    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}

void Aquarium::setBounds(int w, int h){
    m_width = w;
    m_height = h;
    for(auto& c : m_creatures){
        c->setBounds(m_width - 20, m_height - 20);
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();
    int speed = 1 + rand() % 5; // Speed between 1 and 10

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
         case AquariumCreatureType::JellyFish:
            this->addCreature(std::make_shared<JellyFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::JellyFish)));
            break;
         case AquariumCreatureType::FastFish:
            this->addCreature(std::make_shared<FastFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::FastFish)));
            break;
         case AquariumCreatureType::PowerUp:
            {
                auto p = std::make_shared<NPCreature>(x, y, 2, this->m_sprite_manager->GetSprite(AquariumCreatureType::PowerUp));
                p->setCollisionRadius(18);
                p->setCreatureType(AquariumCreatureType::PowerUp);
                p->setSpeed(2);
                this->addCreature(p);
            }
            break;
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // lets make the levels circular
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    //Checks NPC vs Player collisions
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }

    //Checks NPC vs NPC collisions
    int count = aquarium->getCreatureCount();
    for (int i = 0; i < count; ++i){
        std::shared_ptr<Creature> a = aquarium->getCreatureAt(i);
        if(!a || std::static_pointer_cast<NPCreature>(a)->GetType() == AquariumCreatureType::PowerUp) continue;
        for(int j = i + 1; j < count; ++j){
            std::shared_ptr<Creature> b = aquarium->getCreatureAt(j);
            if(!b || std::static_pointer_cast<NPCreature>(b)->GetType() == AquariumCreatureType::PowerUp) continue;
            if(checkCollision(a, b)){
                return std::make_shared<GameEvent>(GameEventType::COLLISION, a, b);
            }
        }
    }
    return nullptr;
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    this->m_player->update();

    if (this->updateControl.tick()) {
        event = DetectAquariumCollisions(this->m_aquarium, this->m_player);
        if (event != nullptr && event->isCollisionEvent()) {
            if(event->creatureA == this->m_player){
               ofLogVerbose() << "Collision detected between player and NPC!" << std::endl;
                if(event->creatureB != nullptr){
                    event->print();
                    //Player vs PowerUp collisions
                    if(std::static_pointer_cast<NPCreature>(event->creatureB)->GetType() == AquariumCreatureType::PowerUp){
                        this->m_player->applySpeedBoost(2, 300);
                        this->m_aquarium->removeCreature(event->creatureB);
                    }
                    //Player vs NPC collisions
                    else if(this->m_player->getPower() < event->creatureB->getValue()){
                        ofLogNotice() << "Player is too weak to eat the creature!" << std::endl;
                        this->m_player->loseLife(3*60); // 3 frames debounce, 3 seconds at 60fps
                        if(this->m_player->getLives() <= 0){
                            this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
                            return;
                        }
                    }
                    else{
                        if(eatSound) eatSound->play();
                        this->m_aquarium->removeCreature(event->creatureB);
                        this->m_player->addToScore(1, event->creatureB->getValue());
                        if (this->m_player->getScore() % 25 == 0){
                            this->m_player->increasePower(1);
                            ofLogNotice() << "Player power increased to " << this->m_player->getPower() << "!" << std::endl;
                        }
                    }
                } else {
                    ofLogError() << "Error: creatureB is null in collision event." << std::endl;
                } 
            }else{//NPC vs NPC collisions
                if(event->creatureA && event->creatureB){
                    if(collisionSound) collisionSound->play();
                    event->creatureA->bounce(event->creatureB);
                }
            }  
        } 
            
    }
    this->m_aquarium->update();
}



void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    float panelHeight = ofGetWindowHeight() - 500;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    if(this->m_player->isBoostActive()){
        ofSetColor(ofColor::lightGreen);
        ofDrawBitmapString("YOU HAVE 2X SPEED!", ofGetWindowWidth()/2 - 75, panelHeight);
    }
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}




std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}


std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_3::Repopulate(){
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta > 0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}
