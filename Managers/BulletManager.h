#pragma once
#include "Bullet.h"
class BulletManager : public Manager
{
public:
	vector<Bullet> bullets;
	void LoadBulletsFromCall(int prim_count, TriListVertex *vs, RenderInfo info);
	void PrintAllBullets(ofstream &s);
	BulletManager();
	~BulletManager();
};

