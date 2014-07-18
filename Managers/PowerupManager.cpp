#include "Main.h"
#include "PowerupManager.h"

const float EPSILON = 0.00001f;

Powerup::Powerup(TriListVertex *vertices, RenderInfo info) {
	TriListVertex upper_left_list = vertices[0];
	TriListVertex upper_right_list = vertices[1];
	TriListVertex lower_left_list = vertices[2];
	TriListVertex lower_right_list = vertices[5];

	/*if (!(vertices[3].p == upper_right_list.p && vertices[4].p == lower_left_list.p)) {
		throw BulletOffScreenException();
	}*/

	Vec2f upper_left = Vec2f(upper_left_list.p.x, upper_left_list.p.y);
	Vec2f upper_right = Vec2f(upper_right_list.p.x, upper_right_list.p.y);
	Vec2f lower_left = Vec2f(lower_left_list.p.x, lower_left_list.p.y);
	Vec2f lower_right = Vec2f(lower_right_list.p.x, lower_right_list.p.y);

	Vec2f upperside = upper_right - upper_left;
	center = (upper_left + lower_right) / 2.0;

	float u = upper_left_list.uv.x;
	float v = upper_left_list.uv.y;
	powerupType = PowerupTypeFor(u, v);
}

Powerup::~Powerup() {}

PowerupType Powerup::PowerupTypeFor(float u, float v) {
	int col = (int)((u + EPSILON) * 16);
	int row = (int)((v + EPSILON) * 16);
	if (row != 13) return NOT_A_POWERUP;
	col = col % 8;
	return (PowerupType)col;
}

PowerupManager::PowerupManager() {}
PowerupManager::~PowerupManager() {}

void PowerupManager::LoadPowerupsFromCall(RenderInfo &info) {
	int numPrimitives = info.PrimitiveCount;
	TriListVertex *vs = (TriListVertex *)info.UserVertexData;

	for (int i = 0; i < numPrimitives - 1; i += 2) {
		Powerup p = Powerup(&vs[i * 3], info);
		if (p.powerupType != NOT_A_POWERUP) {
			powerups.push_back(p);
		}
	}
}

void PowerupManager::StartFrame() {
	powerups.clear();
}

void PowerupManager::EndFrame() {
	g_Context->WriteConsole(String("Powerups: ") + String(powerups.size()), RGBColor::Green, OverlayPanelStatus);
}