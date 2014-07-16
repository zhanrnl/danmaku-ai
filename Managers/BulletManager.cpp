#include "Main.h"
#include "BulletManager.h"

void BulletManager::LoadBulletsFromCall(int numPrimitives, TriListVertex *vs, RenderInfo info, UINT frameIndex) {
	if (frameIndex != currentFrameIndex) {
		prevBullets = move(bullets);
	}
	for (int i = 0; i < numPrimitives / 2; i++) {
		Bullet b = Bullet(&vs[i * 6], info);
		if (b.IsDeadly()) {
			bullets.push_back(b);
		}
	}
}

void BulletManager::PrintAllBullets(ofstream &s) {
	for (Bullet b : bullets) {
		b.Print(s);
	}
}

BulletManager::BulletManager() {}

BulletManager::~BulletManager() {}
