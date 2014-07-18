#pragma once
#include "Bullet.h"
class BulletManager : public Manager
{
public:
	void LoadBulletsFromCall(RenderInfo &info);
	void PrintAllBullets(ofstream &s);
	BulletManager();
	~BulletManager();

	const vector<Bullet> &getBullets() {
		return bullets;
	}
	const Vec2f *getBossLocation(); // return null if no boss exists

	void StartFrame();
	void EndFrame();

private:
	void UpdateBullets();
	static bool IsValidBucket(int x, int y);

	vector<Bullet> bullets;
	vector<Bullet> prevBullets;
	
	UINT bulletsHWM = 0;
	UINT numEnemies = 0;
	Vec2f bossLocation;
	bool bossExists;
};

