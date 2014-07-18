#pragma once

class CharacterManager :
	public Manager
{
public:
	static const Vec2f NO_POSITION;
	Vec2f getPosition() {
		return position;
	}
	UINT getLives() {
		return lives;
	}
	UINT getBombs() {
		return power;
	}
	bool getWasDead() {
		return wasDead;
	}
	void UpdateCharacterFromCall(RenderInfo &info);
	void UpdateScoreFromCall(RenderInfo &info);
	void UpdateIsDeadFromCall(RenderInfo &info);

	void EndFrame();

	CharacterManager();
	~CharacterManager();

private:
	Vec2f position;
	UINT lives;
	UINT power; // rounded down
	bool dead; // whether the character is dead this frame
	bool wasDead; // whether the character was dead last frame (used to determine when to deathbomb)
};

