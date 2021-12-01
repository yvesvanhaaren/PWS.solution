#pragma once
#include "Vectors.h"
static class ModelData
{
public:
	static const int width = 340;
	static const int height = 460;
	static const int rtrDistance = 20; //To prevent router from being inside the wall (previously 20)
	static const int RayCount = 30;	//Max 60
	static const int Threads = 16;	//Max 16

	enum WallType {
		Default,
		Glass,
		Outer
	};

	struct Wall {
		float height;
		int domains; //domainstart
		int domaine; //domainend
		bool x; //true = x, false = y
		WallType walltype; //walltype.Default
	};

	static const int roomcount = 1;
	struct Room {
		Vector2 lc; //left corner
		Vector2 rc;	//right corner
	};

	static Room GetRoom(int index) {
		Room AllR[roomcount]{
			{{100, 100},{500,300}}
		};
		return AllR[index];
	}

	static const int size = 17;
	static Wall GetWall(int index)
	{
		Wall AllW[size]
		{
			{1, 220, 460, true, Outer},	//y = 10 from x = 10 to x = 590
			{320, 1, 200, false, Default}, //y = 390 from x = 10 to x = 590
			{220, 1, 100, false, Outer},	//x = 10 from y = 100 to y = 300
			{100, 220, 320, true, Default},	//x = 490 from y = 100 to y = 150
			{100, 1, 220, true, Outer},	//x = 490 from y = 100 to y = 150
			{459, 1, 200, false, Outer},	//y = 10 from x = 10 to x = 590
			{200, 320, 459, true, Outer},	//y = 10 from x = 10 to x = 590
			{320, 200, 339, false, Outer}, //y = 390 from x = 10 to x = 590
			{339, 320, 1, true, Outer}, //y = 390 from x = 10 to x = 590
			{1, 100, 340, false, Outer},	//y = 10 from x = 10 to x = 590
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

	static const int StartStrength = 100;

	static int GetStartStrength() { return StartStrength; }

	static float GetReflection(WallType a){
		if (a == Default)
			return 0.3f;
		if (a == Glass)
			return 0.3f;
		if (a == Outer)
			return 0.3f;
		return 0.3f;
	}
	static float GetPermittivity(WallType a) {
		if (a == Default)
			return 0.20f;
		if (a == Glass)
			return 0.50f;
		if (a == Outer)
			return 0.0f;
		return 0.1f;
	}
};

