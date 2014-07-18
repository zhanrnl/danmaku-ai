#include "Main.h"

#include "PlayerManager.h"
#include "PowerupManager.h"

#include <limits>

// Constants for REIMU.
const float ORTHOGONAL_SPEED_FOCUSED = 2;
const float ORTHOGONAL_SPEED_UNFOCUSED = 4;

const float EPSILON = 0.00001f;

const float LOW_Y = 47.5;
const float HIGH_Y = 447.5;
const float LOW_X = 39.5;
const float HIGH_X = 407.5;

const float COLLIDE_WITH_BULLET = -1000.0;

const float BOMB_THRESHOLD = -30000.0;

enum LeftRightMovement {
	LEFT_RIGHT_NONE,
	LEFT,
	RIGHT,
	NUM_LEFT_RIGHT,
};

enum UpDownMovement {
	UP_DOWN_NONE,
	UP,
	DOWN,
	NUM_UP_DOWN,
};

enum FocusOptions {
	NORMAL,
	FOCUS,
	NUM_FOCUS_OPTIONS,
};

PlayerManager::PlayerManager() : gameplay(), utilityLWM(0), slowMode(false) {
	
}

PlayerManager::~PlayerManager() {

}

Vec2f GetNextPosition(Vec2f currentPosition, LeftRightMovement lrm, UpDownMovement udm, FocusOptions fo) {
	Vec2f prospectivePosition = currentPosition;

	float speed = (fo == NORMAL) ? ORTHOGONAL_SPEED_UNFOCUSED : ORTHOGONAL_SPEED_FOCUSED;

	if (lrm != LEFT_RIGHT_NONE && udm != UP_DOWN_NONE) {
		speed /= sqrt(2.0f);
	}

	switch (udm) {
	case UP:
		prospectivePosition += Vec2f(0, -speed);
		if (prospectivePosition.y < LOW_Y) {
			prospectivePosition.y = LOW_Y;
		}
		break;
	case DOWN:
		prospectivePosition += Vec2f(0, speed);
		if (prospectivePosition.y > HIGH_Y) {
			prospectivePosition.y = HIGH_Y;
		}
		break;
	default:
		break;
	}

	switch (lrm) {
	case LEFT:
		prospectivePosition += Vec2f(-speed, 0);
		if (prospectivePosition.x < LOW_X) {
			prospectivePosition.x = LOW_X;
		}
		break;
	case RIGHT:
		prospectivePosition += Vec2f(speed, 0);
		if (prospectivePosition.x > HIGH_X) {
			prospectivePosition.x = HIGH_X;
		}
		break;
	default:
		break;
	}

	return prospectivePosition;
}

float UtilityFromBulletPosition(const Bullet &b, Vec2f bulletPosition, Vec2f position) {
	Vec2f bulletToPosition = position - bulletPosition;
	Vec2f parallelVector = b.velocity * (Vec2f::Dot(bulletToPosition, b.velocity) / b.velocity.LengthSq());
	Vec2f perpendicularVector = bulletToPosition - parallelVector;

	float timeTillMatters = parallelVector.Length() / b.velocity.Length();
	//g_Context->Files.CurrentFrameAllEvents << "BPosition: (" << b->center.x << "," << b->center.y << ") (" << b->velocity.x << "," << b->velocity.y << ")\n";

	//g_Context->Files.CurrentFrameAllEvents << "PVector: " << perpendicularVector.Length() << endl;

	if (perpendicularVector.Length() > 6) return 0;
	if (timeTillMatters > 10 || timeTillMatters < -3) return 0;

	float currentUtility = min(0.0f, -1.0f / (perpendicularVector.Length() * pow(timeTillMatters, 2)));

	float smallerDimension = min(b.dimensions.x, b.dimensions.y) / 2.0f;
	float distanceToBullet = (bulletPosition - position).Length();
	if (distanceToBullet < smallerDimension) {
		currentUtility += COLLIDE_WITH_BULLET * (1 + (smallerDimension / distanceToBullet));
	}

	return currentUtility;
}

float UtilityFromBullet(const Bullet &b, Vec2f position) {
	if (b.velocity.LengthSq() < EPSILON) {
		return 0;
	}

	Vec2f bulletPosition = b.center;// +b->velocity;
	float currentUtility = 0.0;
	currentUtility += UtilityFromBulletPosition(b, bulletPosition, position);
	currentUtility += UtilityFromBulletPosition(b, bulletPosition + b.velocity, position);

	return currentUtility;
}

float UtilityFromPowerupPosition(const Powerup &p, Vec2f position) {
	switch (p.powerupType) {
	case POWER:
	case POWER_LARGE:
	case ONE_UP:
	case SCORE_BLUE:
	case SCORE_BLUE_LARGE:
		break;
	default:
		return 0.0f;
	}
	return -0.1f * (position - p.center).LengthSq();
}

void PlayerManager::EndFrame() {
	Vec2f characterPosition = g_Context->Managers.Character.getPosition();

	if (characterPosition.x == CharacterManager::NO_POSITION.x &&
		characterPosition.y == CharacterManager::NO_POSITION.y) {
		return;
	}

	const vector<Bullet> bullets = g_Context->Managers.Bullet.getBullets();
	const vector<Powerup> powerups = g_Context->Managers.Powerup.getPowerups();
	const UINT bombs = g_Context->Managers.Character.getBombs();

	float utilities[NUM_LEFT_RIGHT][NUM_UP_DOWN][NUM_FOCUS_OPTIONS];

	for (int lrm = 0; lrm < NUM_LEFT_RIGHT; lrm++) {
		for (int udm = 0; udm < NUM_UP_DOWN; udm++) {
			for (int fo = 0; fo < NUM_FOCUS_OPTIONS; fo++) {
				utilities[lrm][udm][fo] = 0.0;

				Vec2f nextPosition = GetNextPosition(characterPosition,
					static_cast<LeftRightMovement>(lrm),
					static_cast<UpDownMovement>(udm),
					static_cast<FocusOptions>(fo));

				for (const Bullet b : bullets) {
					utilities[lrm][udm][fo] += UtilityFromBullet(b, nextPosition);
				}

				for (const Powerup p : powerups) {
					utilities[lrm][udm][fo] += UtilityFromPowerupPosition(p, nextPosition);
				}

				utilities[lrm][udm][fo] += -10.0f / abs(nextPosition.x - LOW_X);
				utilities[lrm][udm][fo] += -10.0f / abs(nextPosition.x - HIGH_X);
				utilities[lrm][udm][fo] += -10.0f / abs(nextPosition.y - HIGH_Y);
				utilities[lrm][udm][fo] += -100.0f / abs(nextPosition.y - LOW_Y);
				//g_Context->Files.CurrentFrameAllEvents << "Utilities: " << utilities[lrm][udm][fo] << endl;
			}
		}
	}

	int bestLrm = 0;
	int bestUdm = 0;
	int bestFo = 0;
	float bestUtility = -std::numeric_limits<float>::infinity();

	for (int lrm = 0; lrm < NUM_LEFT_RIGHT; lrm++) {
		for (int udm = 0; udm < NUM_UP_DOWN; udm++) {
			for (int fo = 0; fo < NUM_FOCUS_OPTIONS; fo++) {
				if (bestUtility < utilities[lrm][udm][fo]) {
					bestUtility = utilities[lrm][udm][fo];
					bestLrm = lrm;
					bestUdm = udm;
					bestFo = fo;
				}
			}
		}
	}

	g_Context->WriteConsole(String("AI: ") + String(bestLrm) + String(bestUdm) + String(bestFo) + String("|") + String(bestUtility), RGBColor::Green, OverlayPanelStatus);

	if (utilityLWM > bestUtility) {
		utilityLWM = bestUtility;
	}
	g_Context->WriteConsole(String("Utility LWM: ") + String(utilityLWM), RGBColor::Cyan, OverlayPanelStatus);

	if (bestUtility <= BOMB_THRESHOLD && bombs > 0) {
		gameplay.bomb();
		return;
	}

	// Send the output
	int movement = 0;

	switch (bestLrm) {
	case LEFT:
		movement |= DIRECTION_LEFT;
		break;
	case RIGHT:
		movement |= DIRECTION_RIGHT;
		break;
	default:
		break;
	}

	switch (bestUdm) {
	case UP:
		movement |= DIRECTION_UP;
		break;
	case DOWN:
		movement |= DIRECTION_DOWN;
		break;
	default:
		break;
	}

	bool shouldFocus = (bestFo == FOCUS);

	//gameplay.move(movement, true, shouldFocus);

	if (slowMode) {
		Sleep(50);
	}
}
