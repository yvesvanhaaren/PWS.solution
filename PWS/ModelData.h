#pragma once
#include "Vectors.h"
static class ModelData
{
public:
	static const int width = 340;
	static const int height = 460;
	static const int rtrDistance = 20; //To prevent rBrick from being inside the wall (previously 20)
	static const int RayCount = 4320;	//Max 60
	static const int Threads = 16;	//Max 16

	enum WallType {
		Default,
		Glass,
		Brick,
		Wood
	};

	struct Wall {
		float height;
		int domains; //domainstart
		int domaine; //domainend
		bool x; //true = x, false = y
		WallType walltype; //walltype.Default
	};

	static const int roomcount = 2;
	struct Room {
		Vector2 lc; //left corner
		Vector2 rc;	//right corner
	};

	static Room GetRoom(int index) {
		Room AllR[roomcount]{
			{{0, 0},{100,220}},
			{{200, 320},{340,460}}
		};
		return AllR[index];
	}
	static int IsOutsideHouse(int x, int y) {
		for (int i = 0; i < roomcount; i++)
		{
			if (GetRoom(i).lc.x <= x && GetRoom(i).rc.x >= x && GetRoom(i).lc.y <= y && GetRoom(i).rc.y >= y)
				return 0;
		}
		return 1;
	}

	static const int size = 17;
	static Wall GetWall(int index)
	{
		Wall AllW[size]
		{
			{0, 220, 460, true, Brick},	//y = 10 from x = 10 to x = 590
			{320, 0, 200, false, Default}, //y = 390 from x = 10 to x = 590
			{220, 0, 100, false, Brick},	//x = 10 from y = 100 to y = 300
			{100, 220, 320, true, Default},	//x = 490 from y = 100 to y = 150
			{100, 0, 220, true, Brick},	//x = 490 from y = 100 to y = 150
			{459, 0, 200, false, Brick},	//y = 10 from x = 10 to x = 590
			{200, 320, 459, true, Brick},	//y = 10 from x = 10 to x = 590
			{320, 200, 339, false, Brick}, //y = 390 from x = 10 to x = 590
			{339, 320, 0, true, Brick}, //y = 390 from x = 10 to x = 590
			{0, 100, 340, false, Brick},	//y = 10 from x = 10 to x = 590
			{190, 1, 320, true, Default}, //y = 390 from x = 10 to x = 590

			{30, 100, 150, false, Default},	//y = 10 from x = 10 to x = 590
			{40, 100, 150, false, Default},	//y = 10 from x = 10 to x = 590
			{100, 100, 150, false, Default},	//y = 10 from x = 10 to x = 590
			{140, 100, 150, false, Default},	//y = 10 from x = 10 to x = 590
			{180, 100, 190, false, Default},	//y = 10 from x = 10 to x = 590
			{150, 1, 180, true, Default}, //y = 390 from x = 10 to x = 590
		};
		return AllW[index];
	}
	static int GetrtrDistance() { return rtrDistance; }
	static int GetRayCount() { return RayCount; }

	static const int StartStrength = 255;

	static int GetStartStrength() { return StartStrength; }

	static int GetLPM() { return 3.9168f; }

	static float GetPermittivity(WallType a) {
		if (a == Default)
			return 7.0f;
		if (a == Glass)
			return 1.6906f;
		if (a == Brick)
			return 14.621f;
		if (a == Wood)
			return 6.1253;
		return 0.1f;
	}
};

