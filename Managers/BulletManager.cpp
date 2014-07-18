#include "Main.h"
#include "BulletManager.h"
#include "TouhouConstants.h"

#include <float.h>

const float EPSILON = 0.00001f;
const float EXCLUDE_ALL_RADIUS = 250.f; // all bullets outside of this radius around player are ignored

bool ShouldIgnoreBullet(const Bullet &b) {
	Vec2f d = g_Context->Managers.Character.getPosition() - b.center;
	float distFromPlayerSq = d.LengthSq();
	if (distFromPlayerSq > EXCLUDE_ALL_RADIUS * EXCLUDE_ALL_RADIUS) {
		return true;
	}
	return false;
}

float GetBulletDistanceSquared(const Bullet &b1, const Bullet &b2) {
	return (b1.center - b2.center).LengthSq();
}

float GetDistanceSquared(const Vec2f &l1, const Vec2f &l2) {
	return (l1 - l2).LengthSq();
}

void BulletManager::UpdateBullets() {
	if (bullets.size() <= 0 || prevBullets.size() <= 0) {
		return;
	}

	KDTree3 kdTree = KDTree3();

	Vector<Vec3f> vec = Vector<Vec3f>();
	for (Bullet &b : prevBullets) {
		Vec3f p = Vec3f(b.center, 0.f);
		vec.PushEnd(p);
	}
	kdTree.BuildTree(vec);

	for (Bullet &b : bullets) {
		UINT nearestI = -1;
		kdTree.KNearest(Vec3f(b.center, 0.f), 1, &nearestI, EPSILON);
		if (nearestI != -1) {
			Vec2f v = b.center - prevBullets[nearestI].center;
			b.velocity = v;
		}
	}
}

void BulletManager::LoadBulletsFromCall(RenderInfo &info) {
	int numPrimitives = info.PrimitiveCount;
	TriListVertex *vs = (TriListVertex *)info.UserVertexData;

	for (int i = 0; i < numPrimitives - 1; i+=2) {
		try {
			Bullet b = Bullet(&vs[i * 3], info);
			if (b.IsDeadly()) {
				bullets.push_back(b);
			}
		}
		catch (BulletOffScreenException ex) {
			i -= 1;
		}
	}
}

void BulletManager::PrintAllBullets(ofstream &s) {
	for (Bullet b : bullets) {
		b.Print(s);
	}
}

void BulletManager::StartFrame() {
	prevBullets = move(bullets);
}

void BulletManager::EndFrame() {
	UpdateBullets();

	g_Context->WriteConsole(String("Bullets: ") + String(bullets.size()), RGBColor::Green, OverlayPanelStatus);
	if (bullets.size() > bulletsHWM) {
		bulletsHWM = bullets.size();
	}
	g_Context->WriteConsole(String("Bullets HWM: ") + String(bulletsHWM), RGBColor::Cyan, OverlayPanelStatus);

#ifndef ULTRA_FAST
	if (g_Context->Files.CurrentFrameAllEvents.is_open()) {
		PrintAllBullets(g_Context->Files.CurrentFrameAllEvents);
	}
#endif
}

BulletManager::BulletManager() {
}

BulletManager::~BulletManager() {}
