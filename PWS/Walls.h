#pragma once
#include "Line.h"
static class Walls
{
public:

	struct Wall {
		float height;
		int domains;
		int domaine;
		bool x; //true = x, false = y
	};
	static Vector2 FindFirstCollision(float a, float b, float beginpointx, float beginpointy)
	{
		const int size = 4;
		Wall AllW[size]
		{
			{100, 100, 500, false},	//y = 100 from x = 100 to x = 500
			{300, 100, 500, false}, //y = 300 from x = 100 to x = 500
			{100, 100, 300, true},	//x = 100 from y = 100 to y = 300
			{500, 100, 300, true}	//x = 500 from y = 100 to y = 300
		};

		Vector2 Endpoints[size];

		for (int i = 0; i < size; i++)
		{
			Endpoints[i] = { INFINITY,INFINITY };
		} //Default endpoints to infinity

		int index = 0;
		for (int i = 0; i < size; i++)
		{
			if (CalculateWallIntersection(AllW[i], a, b, beginpointx, beginpointy) != Vector2{ INFINITY,INFINITY }) 
			{
				Endpoints[index] = CalculateWallIntersection(AllW[i], a, b, beginpointx, beginpointy);
				index++;
			}
		}

		if(index == 0)
			return { a * 1000 + beginpointx, b * 1000 + beginpointy };

		Vector2 nearest = { NAN, NAN };
		float distance = INFINITY;
		for (int i = 0; i <= index; i++)
		{
			if (Endpoints[i].distance({ beginpointx,beginpointy }) < distance)
			{
				nearest = Endpoints[i];
			}
		}
		return nearest;


	}
	static Vector2 CalculateWallIntersection(Wall wall, float a, float b, float beginpointx, float beginpointy) {
		if (wall.x) 
		{
			int domainx = wall.domains;
			int domainy = wall.domaine;
			float e = wall.height;
			float t = (e - beginpointx) / a;
			if (t > 0 && b * t + beginpointy > domainx && b * t + beginpointy < domainy)
			{
				return { t * a + beginpointx, t * b + beginpointy };
			}
		}
		else if (!wall.x) 
		{
			int domainx = wall.domains;
			int domainy = wall.domaine;
			float e = wall.height;
			float t = (e - beginpointy) / b;
			if (t > 0 && a * t + beginpointx > domainx && a * t + beginpointx < domainy)
			{
				return { t * a + beginpointx, t * b + beginpointy };				
			}
		}
		return { INFINITY,INFINITY };
	}
};

