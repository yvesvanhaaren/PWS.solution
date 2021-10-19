#include "SDL.h"

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

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);  //Set the colour to white

        float size = 100;
        for (float phi = 0; phi < 360; phi++)
        {
            SDL_RenderDrawLine(renderer, 300, 200, cos(phi) * size + 300, sin(phi) * size + 200);
        }

        SDL_RenderPresent(renderer);    //Render the above

        while (SDL_PollEvent(&event)) { //This while loop
            if (event.type == SDL_QUIT) {
                done = SDL_TRUE;
            }
        }
    }



	return 0;
}