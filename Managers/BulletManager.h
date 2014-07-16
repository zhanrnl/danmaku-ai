#pragma once
#include "Bullet.h"
class BulletManager : public Manager
{
public:
	vector<Bullet> bullets;
	vector<Bullet> prevBullets;
	UINT currentFrameIndex;
	void LoadBulletsFromCall(int numPrimitives, TriListVertex *vs, RenderInfo info, UINT frameIndex);
	void PrintAllBullets(ofstream &s);
	BulletManager();
	~BulletManager();
};

