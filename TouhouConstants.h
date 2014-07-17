#pragma once

const int SCREEN_WIDTH = 450;
const int SCREEN_HEIGHT = 500;

bool IsOnScreen(const Vec2f &location) {
	return 0 <= location.x && location.x < SCREEN_WIDTH &&
		0 <= location.y && location.y < SCREEN_HEIGHT;
}