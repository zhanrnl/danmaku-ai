#pragma once


enum BulletType {
	LARGE_STAR_BULLET = 0,
	LARGE_CIRCLE_BULLET = 1,
	LARGE_BUTTERFLY_BULLET = 2,
	LARGE_KNIFE_BULLET = 3,
	LARGE_OVAL_BULLET = 4,
	LARGE_BUBBLE_BULLET = 5,
	SMALL_LASER_BULLET = 6,
	SMALL_ARROWHEAD_BULLET = 7,
	SMALL_DOUBLE_CIRCLE_BULLET = 8,
	SMALL_SINGLE_CIRCLE_BULLET = 9,
	SMALL_DIAMOND_BULLET = 10,
	SMALL_KUNAI_BULLET = 11,
	SMALL_SHARD_BULLET = 12,
	SMALL_AMULET_BULLET = 13,
	SMALL_BULLET_BULLET = 14,
	SMALL_DARK_DIAMOND_BULLET = 15,
	SMALL_STAR_BULLET = 16,
	SMALL_MONEY_BULLET = 17,
	SMALL_SHINY_CIRCLE_BULLET = 18,
	TINY_WHITE_BULLET = 19,
	TINY_COLORED_BULLET = 20,
	NOT_DEADLY_BULLET = 21
};

class Bullet {
public:
	Vec2f center;
	Vec2f dimensions;
	float rotation; // in radians
	Vec2f velocity;
	BulletType bulletType;

	Bullet(TriListVertex *vertices, RenderInfo info);
	~Bullet();

	void Print(ofstream &s);
	bool IsDeadly();
	float DistanceTo(const Bullet &other);

};

