#include "Main.h"
#include "BulletManager.h"

void BulletManager::LoadBulletsFromCall(int prim_count, TriListVertex *vs, RenderInfo info){
	bullets.clear();
	for (int i = 0; i < prim_count / 2; i++) {
		Bullet b = Bullet(&vs[i * 6], info);
		bullets.push_back(b);
	}
}

void BulletManager::PrintAllBullets(ofstream &s) {
	for (Bullet b : bullets) {
		b.Print(s);
	}
}

BulletManager::BulletManager()
{
}

BulletManager::~BulletManager()
{
}
