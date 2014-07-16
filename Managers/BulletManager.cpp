#include "Main.h"
#include "BulletManager.h"
#include "TouhouConstants.h"

#include <float.h>

float GetBulletDistanceSquared(Bullet b1, Bullet b2) {

	return (b1.center - b2.center).LengthSq();
}

bool BulletManager::IsValidBucket(int x, int y) {
	return 0 <= x && x < GRID && 0 <= y && y < GRID;
}

void BulletManager::UpdateBullets() {
	if (bullets.size() <= 0) {
		return;
	}

	// Fill in the buckets.
	for (int i = 0; i < GRID; i++) {
		for (int j = 0; j < GRID; j++) {
			buckets[i][j].clear();
		}
	}

	for (UINT i = 0; i < bullets.size(); i++) {
		int x = static_cast<int>(bullets[i].center.x * GRID / (float)SCREEN_WIDTH);
		int y = static_cast<int>(bullets[i].center.y * GRID / (float)SCREEN_HEIGHT);

		if (IsValidBucket(x, y)) {
			buckets[x][y].push_back(bullets[i]);
		}
	}

	int comps = 0;
	for (UINT i = 0; i < prevBullets.size(); i++) {
		Bullet b = prevBullets[i];

		// Do our best to match the bullet.
		float bestDistanceSquared = FLT_MAX;
		Bullet *bestBullet = NULL;

		int x = static_cast<int>(b.center.x * GRID / (float)SCREEN_WIDTH);
		int y = static_cast<int>(b.center.y * GRID / (float)SCREEN_HEIGHT);

		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (!IsValidBucket(x + dx, y + dy)) {
					continue;
				}

				for (Bullet currentBullet : buckets[x + dx][y + dy]) {
					if (b.bulletType != currentBullet.bulletType) {
						continue;
					}
					float distanceSquared = GetBulletDistanceSquared(b, currentBullet);
					if (distanceSquared < bestDistanceSquared) {
						distanceSquared = bestDistanceSquared;
						bestBullet = &currentBullet;
					}
					comps++;
				}
			}
		}

		if (bestBullet != NULL) {
			bestBullet->velocity = b.center - bestBullet->center;
		}
	}
	g_Context->WriteConsole(String("Comps: ") + String(comps), RGBColor::Green, OverlayPanelSystem);
}

void BulletManager::LoadBulletsFromCall(RenderInfo &info, UINT frameIndex) {
	int numPrimitives = info.PrimitiveCount;
	TriListVertex *vs = (TriListVertex *)info.UserVertexData;

	if (frameIndex != currentFrameIndex) {
		prevBullets = move(bullets);
	}
	for (int i = 0; i < numPrimitives / 2; i++) {
		Bullet b = Bullet(&vs[i * 6], info);
		if (b.IsDeadly()) {
			bullets.push_back(b);
		}
	}

	UpdateBullets();
}

void BulletManager::PrintAllBullets(ofstream &s) {
	for (Bullet b : bullets) {
		b.Print(s);
	}
}

BulletManager::BulletManager() {}

BulletManager::~BulletManager() {}
