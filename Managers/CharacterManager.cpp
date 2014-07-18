#include "Main.h"
#include "CharacterManager.h"

const float EPSILON = 0.00001f;
const Vec2f BOMB_COUNT_POSITION = Vec2f(513.5, 127.5);
const Vec2f STAR_UV = Vec2f(0.96875, 0);

const Vec2f CharacterManager::NO_POSITION = Vec2f(-1, -1);

CharacterManager::CharacterManager() {
	position = NO_POSITION;
}

CharacterManager::~CharacterManager() {}

void CharacterManager::UpdateCharacterFromCall(RenderInfo &info) {
	int numPrimitives = info.PrimitiveCount;
	TriListVertex *vertices = (TriListVertex *)info.UserVertexData;
	for (int i = 0; i < numPrimitives * 3; i += 6) {
		TriListVertex upper_left = vertices[i];
		TriListVertex lower_right = vertices[i+5];

		bool isCharacterSprite = upper_left.uv.y < 0.5;
		if (isCharacterSprite){
			Vec4f pos4 = (upper_left.p + lower_right.p) / 2;
			position = Vec2f(pos4.x, pos4.y);
			break;
		}
	}
}

void CharacterManager::UpdateIsDeadFromCall(RenderInfo &info) {
	int numPrimitives = info.PrimitiveCount;
	TriListVertex *vertices = (TriListVertex *)info.UserVertexData;
	for (int i = 0; i < numPrimitives * 3; i += 6) {
		TriListVertex upper_left = vertices[i];
		TriListVertex lower_right = vertices[i + 5];

		Vec4f center4d = (upper_left.p + lower_right.p) / 2;
		Vec2f center = Vec2f(center4d.x, center4d.y);
		if ((center - position).LengthSq() <= 36) {
			int col = (int)((EPSILON + upper_left.uv.x) * 16);
			int row = (int)((EPSILON + upper_left.uv.y) * 16);

			if (row == 1) {
				dead |= (col == 8 || col == 12);
			}
			else if (row == 5) {
				dead |= (col == 0 || col == 4);
			}
		}
	}
}

UINT UVToDigit(Vec2f uv) {
	float u = uv.x;
	return (UINT)((u * 32.f) + EPSILON) - 21;
}

void CharacterManager::UpdateScoreFromCall(RenderInfo &info) {
	g_Context->Files.CurrentFrameAllEvents << "SCORE" << endl;
	int numPrimitives = info.PrimitiveCount;
	TriListVertex *vertices = (TriListVertex *)info.UserVertexData;

	bool setBombs = false;
	int foundStars = 0;

	for (int i = 0; i < numPrimitives * 3; i += 6) {
		TriListVertex upper_left = vertices[i];
		Vec2f upperLeftP2 = Vec2f(upper_left.p.x, upper_left.p.y);
		
		if ((BOMB_COUNT_POSITION - upperLeftP2).LengthSq() < EPSILON) {
			power = UVToDigit(upper_left.uv);
			setBombs = true;
		}

		if ((STAR_UV - upper_left.uv).LengthSq() < EPSILON) {
			foundStars++;
		}
	}

	if (setBombs) {
		lives = foundStars;
	}

}

void CharacterManager::EndFrame() {
	//g_Context->WriteConsole(String("Lives: ") + String(lives), RGBColor::Yellow, OverlayPanelStatus);
	//g_Context->WriteConsole(String("Bombs: ") + String(power), RGBColor::Yellow, OverlayPanelStatus);
	wasDead = dead;
	dead = false;
}