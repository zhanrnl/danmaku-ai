#include "Main.h"

#include "PlayerManager.h"

PlayerManager::PlayerManager() : gameplay() {
	
}

PlayerManager::~PlayerManager() {

}

void PlayerManager::EndFrame() {
	
	gameplay.move(0, true, true);
}