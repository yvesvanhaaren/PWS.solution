#pragma once
static class ModelData
{
public:
	static const int width = 600;
	static const int height = 400;
	static const int accuracy = 5;
	static const int rtrDistance = 21; //To prevent router from being inside the wall (previously 20)

	enum WallType {
		Default
	};

	struct Wall {
		float height;
		int domains; //domainstart
		int domaine; //domainend
		bool x; //true = x, false = y
		WallType walltype;
	};

	static const int size = 4;


	static Wall GetWall(int index)
	{
		Wall AllW[size]
		{
			{100, 100, 500, false, Default},	//y = 100 from x = 100 to x = 500
			{300, 100, 500, false, Default}, //y = 300 from x = 100 to x = 500
			{100, 100, 300, true, Default},	//x = 100 from y = 100 to y = 300
			{500, 100, 300, true, Default}	//x = 500 from y = 100 to y = 300
		};
		return AllW[index];
	}
	static int GetrtrDistance() {
		return rtrDistance;
	}
	static float GetAccuracy() {
		return accuracy;
	}
};

