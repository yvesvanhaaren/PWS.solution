#include "SDL.h"
#include "Walls.h"
#include "Vectors.h"
#include "ModelData.h"

void SaveScreenshot(SDL_Renderer* renderer);          //Declare function so we can use them in main
SDL_Color GetPixel(SDL_Surface* srf, int x, int y);
Uint32 get_pixel32(SDL_Surface* surface, int x, int y);
void CalculateCoverage(SDL_Renderer* renderer);

const int width = ModelData::width;
const int height = ModelData::height;

int main(int argc, char* argv[])
{

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Wi-Fi router location optimizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  
    SDL_RenderClear(renderer);  

    /* ^^^^  Initialize SDL  */


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);  //Set the colour to white
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    float accuracy = 2;
    float size = 250;
    float beginpointx = 0.5 * width;
    float beginpointy = 0.5 * height;

    for (float phi = 0; phi < 360; phi += (1 / accuracy))
    {
        Vector2 Endpoint = Walls::FindFirstCollision(cos(phi), sin(phi), beginpointx, beginpointy);
        SDL_RenderDrawLine(renderer, (int)beginpointx, (int)beginpointy, (int)Endpoint.x, (int)Endpoint.y);
    }

    SDL_RenderPresent(renderer);    //Render the above

    CalculateCoverage(renderer);

    //Draw all the walls
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < ModelData::size; i++)
    {
        ModelData::Wall toDraw = ModelData::GetWall(i);
        //Draw the wall to screen
        if (toDraw.x){
            SDL_RenderDrawLine(renderer, (int)toDraw.height, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine);
        }

        if (!toDraw.x)
        {
            SDL_RenderDrawLine(renderer, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine, (int)toDraw.height);
        }
        
    }
    SDL_RenderPresent(renderer);    //Render the above
   
    SaveScreenshot(renderer); //Save a screenshot with the name "name"

    //Handle mouse movement and quit upon keypress
    bool isRunning = true;
    while (isRunning) {
        SDL_Event event;
        // Start our event loop
        while (SDL_PollEvent(&event)) {
            // Handle each specific event
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            if (event.type == SDL_KEYDOWN) {
                std::cout << "a key has been pressed\n";
                SDL_DestroyWindow(window);
                isRunning = false;
            }
        }
    }
    return 0;
}


void SaveScreenshot(SDL_Renderer* renderer) {
    //Save a screenshot
    const Uint32 format = SDL_PIXELFORMAT_ARGB8888;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
    SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);      //Change this null to a rect to calculate for different rooms

    if (SDL_SaveBMP(surface, "name.bmp") != 0)
    {        
        // Error saving bitmap
        SDL_Log("SDL_SaveBMP failed: %s\n", SDL_GetError());
    }
}

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

void CalculateCoverage(SDL_Renderer* renderer) 
{
    //Calculate coverage
    const Uint32 format = SDL_PIXELFORMAT_ARGB8888;
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format); // * -> heap || no * -> stack
    SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);

    float* coverage = new float[width * height]; // * allocates memory
    coverage[0] = 1;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            SDL_Color col;
            col = GetPixel(surface, i, j);
            coverage[i * height + j] = ((float)col.r + (float)col.b + (float)col.g) / 3.0; //Return coverage value at i, j
        }
    }

    float sum = 0;
    float omega;
    for (int i = 0; i < width * height; i++)
    {
        sum += coverage[i];
    }
    omega = sum / ((float)width * (float)height);
    std::cout << omega;
    //Surface and coverage are never deleted so program uses considerably more memory than necessary
}