#pragma once

class CharacterManager :
	public Manager
{
public:
	Vec2f position;
	UINT lives;
	UINT power; // rounded down
	void UpdateCharacterFromCall(RenderInfo &info);
	void UpdateScoreFromCall(RenderInfo &info);

	void EndFrame();

	CharacterManager();
	~CharacterManager();
};

