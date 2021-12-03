#pragma once
#include "SDL.h"
#include "ModelData.h"

static class CalcCov
{
public:
    SDL_Color GetPixel(SDL_Surface* srf, int x, int y)
    {
        SDL_Color color;

        SDL_GetRGBA(get_pixel32(srf, x, y), srf->format, &color.r, &color.g, &color.b, &color.a);

        return color;
    }
    Uint32 get_pixel32(SDL_Surface* surface, int x, int y)
    {
        //Convert the pixels to 32 bit
        Uint32* pixels = (Uint32*)surface->pixels;

        //Get the requested pixel
        return pixels[(y * surface->w) + x];
    }

    SDL_Surface* coveragesurface;
    int width = ModelData::width;
    int height = ModelData::height;
    float* coverage = new float[width * height]; // * allocates memory

    float CalculateCoverage(SDL_Renderer* renderer)
    {
        //Calculate coverage
        const Uint32 format = SDL_PIXELFORMAT_ARGB8888;
        coveragesurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format); // * -> heap || no * -> stack
        SDL_RenderReadPixels(renderer, NULL, format, coveragesurface->pixels, coveragesurface->pitch);

        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                SDL_Color col;
                col = GetPixel(coveragesurface, i, j);
                coverage[i * height + j] = ModelData::IsOutsideHouse(i, j) * ((float)col.r + (float)col.b + (float)col.g) / 3.0f; //Return coverage value at i, j
            }
        }

        float sum = 0;
        float omega;
        for (int i = 0; i < width * height; i++)
        {
            sum += coverage[i];
        }
        omega = sum / ((float)width * (float)height);



        SDL_FreeSurface(coveragesurface);
        return omega;
    }
};

