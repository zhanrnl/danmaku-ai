#pragma once
#include "Bullet.h"
class BulletManager : public Manager
{
public:
	void LoadBulletsFromCall(RenderInfo &info);
	void PrintAllBullets(ofstream &s);
	BulletManager();
	~BulletManager();

	const vector<Bullet *> &getBullets() {
		return bullets;
	}

	void StartFrame();
	void EndFrame();

private:
	void UpdateBullets();
	static bool IsValidBucket(int x, int y);

	vector<Bullet *> bullets;
	vector<Bullet *> prevBullets;

	static const int GRID = 50;
	vector<Bullet *> buckets[GRID][GRID];
};

