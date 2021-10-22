#pragma once
static class ModelData
{
public:
	static const int width = 600;
	static const int height = 400;

	struct Wall {
		float height;
		int domains;
		int domaine;
		bool x; //true = x, false = y
	};


	static const int size = 4;


	static Wall GetWall(int index)
	{
		Wall AllW[size]
		{
			{100, 100, 500, false},	//y = 100 from x = 100 to x = 500
			{300, 100, 500, false}, //y = 300 from x = 100 to x = 500
			{100, 100, 300, true},	//x = 100 from y = 100 to y = 300
			{500, 100, 300, true}	//x = 500 from y = 100 to y = 300
		};
		return AllW[index];
	}

};

