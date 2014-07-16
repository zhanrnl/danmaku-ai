#pragma once

#include "Main.h"
#include "OutputLayer/gameplay.h"

class PlayerManager : public Manager
{
public:
	PlayerManager();
	~PlayerManager();

	void EndFrame();

private:
	Gameplay gameplay;
};