#pragma once

enum PowerupType {
	POWER,
	SCORE_BLUE,
	SCORE_GREEN,
	POWER_LARGE,
	SCORE_BLUE_LARGE,
	FULL,
	ONE_UP,
	SCORE_GREEN_SMALL,
	NOT_A_POWERUP,
};

class Powerup {
public:
	Vec2f center;
	Vec2f dimensions = Vec2f(16.f, 16.f);
	PowerupType powerupType;

	Powerup(TriListVertex *vertices, RenderInfo info);
	~Powerup();

	PowerupType PowerupTypeFor(float u, float v);
};

class PowerupManager : public Manager
{
public:
	PowerupManager();
	~PowerupManager();

	void LoadPowerupsFromCall(RenderInfo &info);

	const vector<Powerup> &getPowerups() {
		return powerups;
	}

	void StartFrame();
	void EndFrame();

private:
	vector<Powerup> powerups;
};

