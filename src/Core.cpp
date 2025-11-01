#include "Core.h"


// Creature Inherited Base Behavior
void Creature::setBounds(int w, int h) { m_width = w; m_height = h; }
void Creature::normalize() {
    float length = std::sqrt(m_dx * m_dx + m_dy * m_dy);
    if (length != 0) {
        m_dx /= length;
        m_dy /= length;
    }
}

void Creature::bounce(std::shared_ptr<Creature> other) {
    //bounce off walls
    if(m_x - m_collisionRadius < 0){
        m_x = m_collisionRadius;
        m_dx *= -1;
        setFlipped(!m_flipped);
    } else if(m_x + m_collisionRadius > m_width){
        m_x = m_width - m_collisionRadius;
        m_dx *= -1;
        setFlipped(!m_flipped);
    }

    if(m_y - m_collisionRadius < 0){
        m_y = m_collisionRadius;
        m_dy *= -1;;
    } else if(m_y + m_collisionRadius > m_height){
        m_y = m_height - m_collisionRadius;
        m_dy *= -1;
    }

    //bounce off other creatures
    if(other){
        float dxDiff = m_x - other->m_x;
        float dyDiff = m_y - other->m_y;
        float distSq = dxDiff * dxDiff + dyDiff * dyDiff;
        float minDist = m_collisionRadius + other->m_collisionRadius;

        if (distSq < minDist * minDist){
            float dist = sqrt(distSq);
            if (dist > 0) {
                dxDiff /= dist;
                dyDiff /= dist;

                float overlap = 0.5f * (minDist - dist);
                m_x += dxDiff * overlap;
                m_y += dyDiff * overlap;
                other->m_x -= dxDiff * overlap;
                other->m_y -= dyDiff * overlap;

                
                float dotProduct = (m_dx * dxDiff + m_dy * dyDiff);
                m_dx -= 2 * dotProduct * dxDiff;
                m_dy -= 2 * dotProduct * dyDiff;

                dotProduct = (other->m_dx * dxDiff + other->m_dy * dyDiff);
                other->m_dx -= 2 * dotProduct * dxDiff;
                other->m_dy -= 2 * dotProduct * dyDiff;
            }
        }
    }
}


void GameEvent::print() const {
        
        switch (type) {
            case GameEventType::NONE:
                ofLogVerbose() << "No event." << std::endl;
                break;
            case GameEventType::COLLISION:
                ofLogVerbose() << "Collision event between creatures at (" 
                << creatureA->getX() << ", " << creatureA->getY() << ") and ("
                << creatureB->getX() << ", " << creatureB->getY() << ")." << std::endl;
                break;
            case GameEventType::CREATURE_ADDED:
                ofLogVerbose() << "Creature added at (" 
                << creatureA->getX() << ", " << creatureA->getY() << ")." << std::endl;
                break;
            case GameEventType::CREATURE_REMOVED:
                ofLogVerbose() << "Creature removed at (" 
                << creatureA->getX() << ", " << creatureA->getY() << ")." << std::endl;
                break;
            case GameEventType::GAME_OVER:
                ofLogVerbose() << "Game Over event." << std::endl;
                break;
            case GameEventType::NEW_LEVEL:
                ofLogVerbose() << "New Game level" << std::endl;
            default:
                ofLogVerbose() << "Unknown event type." << std::endl;
                break;
        }
};

// collision detection between two creatures
bool checkCollision(std::shared_ptr<Creature> a, std::shared_ptr<Creature> b) {
    ofVec2f posA(a->getX(), a->getY());
    ofVec2f posB(b->getX(), b->getY());
    float distance = posA.distance(posB);
    float collisionDistance = a->getCollisionRadius() + b->getCollisionRadius();
    return distance < collisionDistance;
};


string GameSceneKindToString(GameSceneKind t){
    switch(t)
    {
        case GameSceneKind::GAME_INTRO: return "GAME_INTRO";
        case GameSceneKind::AQUARIUM_GAME: return "AQUARIUM_GAME";
        case GameSceneKind::GAME_OVER: return "GAME_OVER";
    };
};

std::shared_ptr<GameScene> GameSceneManager::GetScene(string name){
    if(!this->HasScenes()){return nullptr;}
    for(std::shared_ptr<GameScene> scene : this->m_scenes){
        if(scene->GetName() == name){
            return scene;
        }
    }
    return nullptr;
}

void GameSceneManager::Transition(string name){
    if(!this->HasScenes()){return;} // no need to do anything if nothing inside
    std::shared_ptr<GameScene> newScene = this->GetScene(name);
    if(newScene == nullptr){return;} // i dont have the scene so time to leave
    if(newScene->GetName() == this->m_active_scene->GetName()){return;} // another do nothing since active scene is already pulled
    this->m_active_scene = newScene; // now we keep it since this is a valid transition
    return;
}

void GameSceneManager::AddScene(std::shared_ptr<GameScene> newScene){
    if(this->GetScene(newScene->GetName()) != nullptr){
        return; // this scene already exist and shouldnt be added again
    }
    this->m_scenes.push_back(newScene);
    if(m_active_scene == nullptr){
        this->m_active_scene = newScene; // need to place in active scene as its the only one in existance right now
    }
    return;
}

std::shared_ptr<GameScene> GameSceneManager::GetActiveScene(){
    return this->m_active_scene;
}

string GameSceneManager::GetActiveSceneName(){
    if(this->m_active_scene == nullptr){return "";} // something to handle missing activate scenes
    return this->m_active_scene->GetName();
}

void GameSceneManager::UpdateActiveScene(){
    if(!this->HasScenes()){return;} // make sure we have a scene before we try to paint
    this->m_active_scene->Update();

}

void GameSceneManager::DrawActiveScene(){
    if(!this->HasScenes()){return;} // make sure we have something before Drawing it
    this->m_active_scene->Draw();
}


void GameIntroScene::Update(){

}

void GameIntroScene::Draw(){
    this->m_banner->draw(0,0);
}

void GameOverScene::Update(){

}

void GameOverScene::Draw(){
    ofBackgroundGradient(ofColor::red, ofColor::black);
    this->m_banner->draw(0,0);

}