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

	DebugOnlyAssert(vertices[3].p == upper_right_list.p, "We didn't find a full rectangle");
	DebugOnlyAssert(vertices[4].p == lower_left_list.p, "We didn't find a full RECTANGLE");

	Vec2f upper_left = Vec2f(upper_left_list.p.x, upper_left_list.p.y);
	Vec2f upper_right = Vec2f(upper_right_list.p.x, upper_right_list.p.y);
	Vec2f lower_left = Vec2f(lower_left_list.p.x, lower_left_list.p.y);
	Vec2f lower_right = Vec2f(lower_right_list.p.x, lower_right_list.p.y);

	Vec2f upperside = upper_right - upper_left;
	center = (upper_left + lower_right) / 2.0;
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
	// TODO: update this.
	return bulletType != NOT_DEADLY_BULLET;
}

// "Distance" function for evaluating possible frame-to-frame matches.
// Returns square of distance so we don't have to take a square root.
float Bullet::DistanceTo(const Bullet &other) {
	if (bulletType != other.bulletType)
		return FLT_MAX;
	float squaredSum = (center - other.center).LengthSq();
	float dRot = rotation - other.rotation;
	while (dRot >= Math::PI) dRot -= float(2 * Math::PI);
	while (dRot < -Math::PI) dRot += float(2 * Math::PI);
	squaredSum += dRot * dRot * DISTANCE_ROT_SCALE;
	return squaredSum;
}