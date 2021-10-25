#pragma once
#include "ModelData.h"
#include "Vectors.h"

static class Walls
{
public:

	static Vector2 FindFirstCollision(float a, float b, float beginpointx, float beginpointy)
	{
		const int size = ModelData::size;

		Vector2 Endpoints[size];

		for (int i = 0; i < size; i++)
		{
			Endpoints[i] = { INFINITY,INFINITY };
		} //Default endpoints to infinity

		int index = 0;
		for (int i = 0; i < size; i++)
		{
			if (CalculateWallIntersection(ModelData::GetWall(i), a, b, beginpointx, beginpointy) != Vector2{ INFINITY,INFINITY }) 
			{
				Endpoints[index] = CalculateWallIntersection(ModelData::GetWall(i), a, b, beginpointx, beginpointy);
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
				distance = Endpoints[i].distance({ beginpointx,beginpointy });
				nearest = Endpoints[i];
			}
		}
		return nearest;


	}
	static Vector2 CalculateWallIntersection(ModelData::Wall wall, float a, float b, float beginpointx, float beginpointy) {
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

	static bool OnWall (float beginpointx, float beginpointy) 
	{
		for (int i = 0; i < ModelData::size; i++)
		{
			ModelData::Wall wall = ModelData::GetWall(i);
			
			if (wall.x)
			{
				int domainx = wall.domains;
				int domainy = wall.domaine;
				float e = wall.height;
				float t = (e - beginpointx);
				if (t == 0)
				{
					return true;
				}
			}
			else if (!wall.x)
			{
				int domainx = wall.domains;
				int domainy = wall.domaine;
				float e = wall.height;
				float t = (e - beginpointy);
				if (t == 0)
				{
					return true;
				}
			}
		}
		return false;
	}
};

