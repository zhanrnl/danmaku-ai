#pragma once
#include "Bullet.h"
class BulletManager : public Manager
{
public:
	UINT currentFrameIndex;
	void LoadBulletsFromCall(RenderInfo &info, UINT frameIndex);
	void PrintAllBullets(ofstream &s);
	BulletManager();
	~BulletManager();

private:
	void UpdateBullets();
	static bool IsValidBucket(int x, int y);

	vector<Bullet> bullets;
	vector<Bullet> prevBullets;

	static const int GRID = 50;
	vector<Bullet> buckets[GRID][GRID];
};

