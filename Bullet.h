#pragma once

#include <exception>

enum BulletType {
	LARGE_STAR_BULLET,
	LARGE_CIRCLE_BULLET,
	LARGE_BUTTERFLY_BULLET,
	LARGE_KNIFE_BULLET,
	LARGE_OVAL_BULLET,
	LARGE_BUBBLE_BULLET,
	LARGE_FUZZY_BULLET,
	SMALL_LASER_BULLET,
	SMALL_ARROWHEAD_BULLET,
	SMALL_DOUBLE_CIRCLE_BULLET,
	SMALL_SINGLE_CIRCLE_BULLET,
	SMALL_DIAMOND_BULLET,
	SMALL_KUNAI_BULLET,
	SMALL_SHARD_BULLET,
	SMALL_AMULET_BULLET,
	SMALL_BULLET_BULLET,
	SMALL_DARK_DIAMOND_BULLET,
	SMALL_STAR_BULLET,
	SMALL_MONEY_BULLET,
	SMALL_SHINY_CIRCLE_BULLET,
	TINY_WHITE_BULLET,
	TINY_COLORED_BULLET,
	ENEMY,
	BOSS,
	NOT_DEADLY_BULLET
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
	bool IsDeadly() const;
	bool IsEnemy() const;
	bool IsBoss() const;
	float DistanceTo(const Bullet &other);

};

class BulletOffScreenException : public exception {

};