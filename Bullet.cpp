#include "Main.h"
#include <cmath>
#include "Bullet.h"

const float EPSILON = 0.00001f;
const float DISTANCE_ROT_SCALE = 200.f;

BulletType BulletTypeFor(UINT textureHash, float u, float v) {
	BulletType bulletType;
	int row, col;
	switch (textureHash) {
	case BULLETS1_HASH:
		col = (int)((u + EPSILON) * 16);
		row = (int)((v + EPSILON) * 16);
		if (row < 12) {
			bulletType = (BulletType)((int)SMALL_LASER_BULLET + row);
		}
		else if (row == 12) {
			if (col < 4) {
				bulletType = TINY_COLORED_BULLET;
			}
			else {
				bulletType = SMALL_MONEY_BULLET;
			}
		}
		else if (row == 15) {
			if (col < 4) {
				bulletType = TINY_WHITE_BULLET;
			}
			else {
				bulletType = SMALL_SHINY_CIRCLE_BULLET;
			}
		}
		else {
			bulletType = NOT_DEADLY_BULLET;
		}
		break;
	case BULLETS2_HASH:
		row = (int)((v + EPSILON) * 8);
		if (row >= 6) {
			bulletType = LARGE_BUBBLE_BULLET;
		}
		else if (row == 5) {
			bulletType = NOT_DEADLY_BULLET;
		}
		else {
			bulletType = (BulletType)row;
		}
		break;
	case POWERUP_HASH:
		row = (int)((v + EPSILON) * 16);
		if (row >= 9 && row < 13) {
			bulletType = LARGE_FUZZY_BULLET;
		}
		else {
			bulletType = NOT_DEADLY_BULLET;
		}
	}
	return bulletType;
}

Bullet::Bullet(TriListVertex *vertices, RenderInfo info) {
	TriListVertex upper_left_list = vertices[0];
	TriListVertex upper_right_list = vertices[1];
	TriListVertex lower_left_list = vertices[2];
	TriListVertex lower_right_list = vertices[5];

	if (!(vertices[3].p == upper_right_list.p && vertices[4].p == lower_left_list.p)) {
		throw BulletOffScreenException();
	}

	Vec2f upper_left = Vec2f(upper_left_list.p.x, upper_left_list.p.y);
	Vec2f upper_right = Vec2f(upper_right_list.p.x, upper_right_list.p.y);
	Vec2f lower_left = Vec2f(lower_left_list.p.x, lower_left_list.p.y);
	Vec2f lower_right = Vec2f(lower_right_list.p.x, lower_right_list.p.y);

	Vec2f upperside = upper_right - upper_left;
	center = (upper_left + lower_right) / 2.0;
	//if (floor(center.x * 2 + 2 * EPSILON) >= EPSILON ||
	//	floor(center.y * 2 + 2 * EPSILON) >= EPSILON) {
	//	g_Context->WriteConsole(String("Non-aligned bullet:") + String(center.x) + String(",") + String(center.y), RGBColor::Green, OverlayPanelStatus);
	//}
	dimensions = Vec2f(upperside.Length(), (upper_left - lower_left).Length());

	rotation = atan2(upperside.y, upperside.x);
	velocity = Vec2f(0.0f,0.0f);
	
	float u = upper_left_list.uv.x;
	float v = upper_left_list.uv.y;
	bulletType = BulletTypeFor(info.Texture0->BmpHash(), u, v);
}

Bullet::~Bullet() {}

void Bullet::Print(ofstream &s) {
	s << "Position {" << center.x << ", " << center.y << "}  Dimensions {" << dimensions.x << ", " << dimensions.y << "}  Rotation " << rotation << "  Type {" << bulletType << "}" << endl;
}

bool Bullet::IsDeadly() {
	return bulletType != NOT_DEADLY_BULLET;
}
