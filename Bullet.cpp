#include "Main.h"
#include <cmath>
#include "Bullet.h"

const float EPSILON = 0.00001;

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
	speed = 0.0;
	
	float u = upper_left_list.uv.x;
	float v = upper_left_list.uv.y;

	switch (info.Texture0->BmpHash()) {
	case BULLETS1_HASH:
		int col = (int)((u + EPSILON) * 16);
		int row = (int)((v + EPSILON) * 16);
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
		// TODO
		break;
	}
}

Bullet::~Bullet() {}

void Bullet::Print(ofstream &s) {
	s << "Position {" << center.x << ", " << center.y << "}  Dimensions {" << dimensions.x << ", " << dimensions.y << "}  Rotation " << rotation << "  Type {" << bulletType << "}" << endl;
}
