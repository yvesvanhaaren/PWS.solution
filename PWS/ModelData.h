#pragma once
#include "Vectors.h"
static class ModelData
{
public:
	static const int width = 600;
	static const int height = 400;
	static const int rtrDistance = 20; //To prevent router from being inside the wall (previously 20)

	enum WallType {
		Default,
		Glass
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
			{10, 10, 590, false, Default},	//y = 10 from x = 10 to x = 590
			{390, 10, 590, false, Default}, //y = 390 from x = 10 to x = 590
			{10, 10, 390, true, Default},	//x = 10 from y = 100 to y = 300
			{590, 10, 390, true, Default},	//x = 490 from y = 100 to y = 150
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			{330, 300, 590, false, Default},	//y = 200 from y = 100 to y = 300
			{400, 10, 390, true, Default},	//x = 490 from y = 100 to y = 150

			{300, 360, 390, true, Default},	//x = 300 from y = 100 to y = 300
			{300, 120, 330, true, Default},	//x = 300 from y = 100 to y = 300

			{250, 370, 390, true, Default},	//x = 400 from y = 100 to y = 300
			{250, 280, 370, true, Glass},	//x = 200 from y = 100 to y = 300
			{250, 250, 280, true, Default},	//x = 400 from y = 100 to y = 300

			{250, 10, 30, false, Default},	//y = 200 from y = 100 to y = 300
			{250, 30, 230, false, Glass},	//y = 200 from y = 100 to y = 300
			{250, 230, 250, false, Default},	//y = 200 from y = 100 to y = 300

			{120, 10, 30, false, Default},	//y = 200 from y = 100 to y = 300
			{120, 30, 230, false, Glass},	//y = 200 from y = 100 to y = 300
			{120, 230, 300, false, Default}	//y = 200 from y = 100 to y = 300
		};
		return AllW[index];
	}
	static int GetrtrDistance() {
		return rtrDistance;
	}
	static float GetReflection(WallType a){
		if (a == Default)
			return 0.4f;
		if (a == Glass)
			return 0.2f;
		return 0.4f;
	}
	static float GetPermittivity(WallType a) {
		if (a == Default)
			return 0.20f;
		if (a == Glass)
			return 0.40f;
		return 0.1f;
	}
};

