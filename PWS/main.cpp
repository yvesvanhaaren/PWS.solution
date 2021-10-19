#include "SDL.h"

void SaveScreenshot(SDL_Surface* surface);          //Declar function so we can use them in main
Uint32 getpixel(SDL_Surface* surface, int x, int y);

int main(int argc, char* argv[])
{

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_bool done = SDL_FALSE;


    while (!done) {
        SDL_Event event;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  //Vertel welke kleur gerendered moet worder

        SDL_RenderClear(renderer);  //Draw that colour across the entire screen

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);  //Set the colour to white
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        float accuracy = 2;
        float size = 250;

        for (float phi = 0; phi < 360; phi += (1/accuracy))
        {
            SDL_RenderDrawLine(renderer, 300, 200, cos(phi) * size + 300, sin(phi) * size + 200);
        }

        SDL_RenderPresent(renderer);    //Render the above

        //Save screen data
        const Uint32 format = SDL_PIXELFORMAT_ARGB8888;
        const int width = 600;
        const int height = 400;

        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
        SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);      //Change this null to a rect to calculate for different rooms


        SaveScreenshot(surface); //Save a screenshot with the name "name"
       
        //Calculate coverage
        float coverage[24000];

        for (int i = 1; i < surface->w; i++)
        {
            for  (int j = 1; j < surface->h; j++) //Loop door alle pixels
            {
                SDL_Color rgb;
                Uint32 data = getpixel(surface, i, j);
                SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);
                coverage[i*surface->w + j] = (rgb.r + rgb.b + rgb.g) * (1 / 3);
            }
        }

        SDL_Delay(3000);

        while (SDL_PollEvent(&event)) { //This while loop ??? doet iets niet weghalen
            if (event.type == SDL_QUIT) {
                done = SDL_TRUE;
            }
        }
    }


	return 0;
}

Uint32 getpixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16*)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32*)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void SaveScreenshot(SDL_Surface* surface) {
    //Save a screenshot
    if (SDL_SaveBMP(surface, "name.bmp") != 0)
    {
        // Error saving bitmap
        SDL_Log("SDL_SaveBMP failed: %s\n", SDL_GetError());
    }
}

