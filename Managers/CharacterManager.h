#pragma once

class CharacterManager :
	public Manager
{
public:
	Vec2f getPosition() {
		return position;
	}
	UINT getLives() {
		return lives;
	}
	UINT getBombs() {
		return power;
	}
	void UpdateCharacterFromCall(RenderInfo &info);
	void UpdateScoreFromCall(RenderInfo &info);

	void EndFrame();

	CharacterManager();
	~CharacterManager();

private:
	Vec2f position;
	UINT lives;
	UINT power; // rounded down
};

