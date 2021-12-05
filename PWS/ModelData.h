#pragma once
#include "Vectors.h"
static class ModelData
{
public:
	static const int width = 340;
	static const int height = 460;
	static const int rtrDistance = 20; //To prevent rBrick from being inside the wall (previously 20)
	static const int RayCount = 7200;	//Max 60
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

	static const int size = 50;
	static Wall GetWall(int index)
	{
		Wall AllW[size]
		{
			{0, 456, 460, true, Brick},	
			{0, 456, 324, true, Glass},	
			{0, 324, 320, true, Brick},	
			{0, 320, 288, true, Wood},	
			{0, 288, 268, true, Brick},	
			{0, 268, 252, true, Glass},	
			{0, 252, 220, true, Brick},	

			{220, 0, 12, false, Brick},	 
			{220, 12, 88, false, Glass},	 
			{220, 88, 100, false, Brick},	 

			{459, 0, 8, false, Brick},	
			{459, 8, 196, false, Glass},	
			{459, 196, 200, false, Brick},	
			
			{320, 0, 40, false, Brick},
			{320, 40, 72, false, Wood},
			
			{320, 72, 100, false, Brick},
			{320, 190, 200, false, Brick},

			{100, 220, 320, true, Brick},

			{100, 0, 220, true, Brick},	 

			{0, 100, 114, false, Brick},
			{0, 114, 140, false, Wood},
			{0, 140, 154, false, Brick},
			{0, 154, 332, false, Glass},
			{0, 332, 339, false, Brick},

			{339, 320, 0, true, Brick},

			{320, 332, 339, false, Brick},
			{320, 208, 332, false, Glass},
			{320, 200, 208, false, Brick},

			{200, 320, 324, true, Brick},
			{200, 324, 455, true, Glass},
			{200, 455, 459, true, Brick},
 
			{190, 1, 180, true, Brick},  
			{190, 180, 212, true, Glass},
			{190, 212, 320, true, Brick},

			{150, 30, 54, true, Brick},
			{150, 54, 82, true, Wood},
			{150, 82, 100, true, Brick},

			{150, 140, 144, true, Brick},
			{150, 144, 176, true, Wood},
			{150, 176, 180, true, Brick},

			{30, 100, 116, false, Brick},
			{30, 116, 150, false, Wood},

			{40, 100, 150, false, Brick},	
			{100, 100, 150, false, Brick},

			{140, 100, 154, false, Brick},	
			{140, 154, 186, false, Wood},
			{140, 186, 190, false, Brick},

			{180, 100, 150, false, Brick},	

		};
		return AllW[index];
	}
	static int GetrtrDistance() { return rtrDistance; }
	static int GetRayCount() { return RayCount; }

	static const int StartStrength = 224.4f;

	static int GetStartStrength() { return StartStrength; }

	static int GetLPM() { return 10.56; }

	static float GetPermittivity(WallType a) {
		if (a == Default)
			return 7.0f;
		if (a == Glass)
			return 4.335f;
		if (a == Brick)
			return 37.23;
		if (a == Wood)
			return 15.555;
		return 0.1f;
	}
};

