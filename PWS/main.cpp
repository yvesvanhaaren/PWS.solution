#include "SDL.h"
#include "Walls.h"
#include "Vectors.h"
#include "ModelData.h"
#include <string>

void SaveScreenshot(SDL_Renderer* renderer, std::string name);
float CalculateCoverage(SDL_Renderer* renderer);
void DrawRouter(SDL_Renderer* renderer, int bx, int by);
void DrawWalls(SDL_Renderer* renderer);
void ClearRenderer(SDL_Renderer* renderer);
SDL_Color GetPixel(SDL_Surface* srf, int x, int y);
Uint32 get_pixel32(SDL_Surface* surface, int x, int y);

const int width = ModelData::width;
const int height = ModelData::height;
float accuracy = (float)ModelData::GetAccuracy();
int rtrDistance = ModelData::GetrtrDistance();

struct RouterConfig {
    Vector2 Location;
    float Coverage;
};
int main(int argc, char* argv[])
{
    //Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Wi-Fi router location optimizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    ClearRenderer(renderer);
    
    //Simulate Wi-Fi router
    RouterConfig OptimalSpot = { {0,0}, 0 };
    for (int i = 0; i < width; i += rtrDistance)
    {
        for (int j = 0; j < height; j += rtrDistance)
        {
            DrawRouter(renderer, i, j);
            //Render the above
            SDL_RenderPresent(renderer);

            //Calculate the coverage of the current setup
            float Coverage = CalculateCoverage(renderer);
            if (Coverage > OptimalSpot.Coverage) {
                RouterConfig currentConfig;
                currentConfig.Coverage = Coverage;
                currentConfig.Location = { (float)i,(float)j };
                OptimalSpot = currentConfig;
            }             
            //OPTIONAL save screenshot of every configuration
            //SaveScreenshot(renderer, std::to_string(i) + "_" + std::to_string(j));

            ClearRenderer(renderer);
        }
    }
    //Final display
    //Draw optimal routerconfig
    DrawRouter(renderer, OptimalSpot.Location.x, OptimalSpot.Location.y);
    //Draw the walls
    DrawWalls(renderer);
    //Render the above
    SDL_RenderPresent(renderer);    
    //Print configuration efficiency to the console
    std::cout << OptimalSpot.Coverage << " is  the coverage at the optimal location (" << OptimalSpot.Location.x << ", " << OptimalSpot.Location.y << ")";
    //Save a screenshot with the name "OptimalLocation"
    SaveScreenshot(renderer, "OptimalLocation");

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
void DrawRouter(SDL_Renderer* renderer, int bx, int by) 
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, (255 / accuracy));

    for (float phi = 0; phi < 360; phi += (1 / accuracy))
    {
        Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), bx, by);
        SDL_RenderDrawLine(renderer, bx, by, e.x, e.y);
    }
}
void ClearRenderer(SDL_Renderer* renderer) 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

void DrawWalls(SDL_Renderer* renderer) 
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < ModelData::size; i++)
    {
        ModelData::Wall toDraw = ModelData::GetWall(i);
        //Draw the wall to screen
        if (toDraw.x) {
            SDL_RenderDrawLine(renderer, (int)toDraw.height, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine);
        }

        if (!toDraw.x)
        {
            SDL_RenderDrawLine(renderer, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine, (int)toDraw.height);
        }

    }
}


void SaveScreenshot(SDL_Renderer* renderer, std::string name) {
    //Save a screenshot
    const Uint32 format = SDL_PIXELFORMAT_ARGB8888;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
    SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);      //Change this null to a rect to calculate for different rooms

    name += ".bmp";
    
    if (SDL_SaveBMP(surface, name.c_str()) != 0)
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

float CalculateCoverage(SDL_Renderer* renderer) 
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
            coverage[i * height + j] = ((float)col.r + (float)col.b + (float)col.g) / 3.0f; //Return coverage value at i, j
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
    std::cout << "\n";
    return omega;
    //Surface and coverage are never deleted so program uses considerably more memory than necessary
}