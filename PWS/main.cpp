#include "SDL.h"
#include "Walls.h"
#include "Vectors.h"
#include "ModelData.h"
#include "CalcCov.h"
#include <string>

struct RouterConfig {
    Vector2 Location;
    float Coverage;
};
SDL_BlendMode lighten = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_COLOR, SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDOPERATION_MAXIMUM, 
                                                    SDL_BLENDFACTOR_SRC_COLOR, SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDOPERATION_ADD);

void CalculatePosition(SDL_Renderer* renderer, int i, int j);
void DrawPermittivity(SDL_Renderer* renderer, float bx, float by, float ex, float ey, int WifiStrength, float angle);
void DrawRoom(SDL_Renderer* renderer, int index, int intensity);
void DrawRect(SDL_Renderer* renderer, int x1, int y1, int x2, int y2);
void SaveScreenshot(SDL_Renderer* renderer, std::string name);
void DrawRouter(SDL_Renderer* renderer, int bx, int by);
void DrawWalls(SDL_Renderer* renderer);
void ClearRenderer(SDL_Renderer* renderer);
float DrawLine(SDL_Renderer* renderer, float bx, float by, float ex, float ey, float WifiStrength, float angle);

const int width = ModelData::width;
const int threads = ModelData::Threads;
const int height = ModelData::height;
const int CPUcount = SDL_GetCPUCount();
const float accuracy = (2 * M_PI) / (ModelData::GetRayCount());
int rtrDistance = ModelData::GetrtrDistance();

struct ThreadData {
    int x;
    int y;
    int index;
};

SDL_Window* window[threads];
SDL_Renderer* renderer[threads];


int Thread_func(void* data) 
{
    ThreadData *tdata = (ThreadData*)data;
    int i = tdata->x;
    int j = tdata->y;
    int index = tdata->index;
    int threadReturnValue = 0;

    SDL_SetRenderDrawBlendMode(renderer[index], lighten);
    ClearRenderer(renderer[index]);

    CalculatePosition(renderer[index], i, j);
    delete tdata;
    return 0;
}

RouterConfig OptimalSpot = { {0,0}, 0 };

int main(int argc, char* argv[])
{
    
    //Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    for (int i = 0; i < threads; i++)
    {
        window[i] = SDL_CreateWindow("Wi-Fi router location optimizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN); 
        renderer[i] = SDL_CreateRenderer(window[i], -1, SDL_RENDERER_ACCELERATED);
    }
 
    int ActiveThreads = 0;
    int threadReturnValue = 0;
    //Simulate Wi-Fi router
    for (int i = rtrDistance; i < width; i += rtrDistance)
    {
        for (int j = rtrDistance; j < height; j += rtrDistance)
        {
            if (!Walls::OnWall(i, j, true) && ModelData::IsOutsideHouse(i, j) == 1)
            {
                SDL_Thread* thread[threads];
                if (ActiveThreads < threads)
                {
                    ThreadData* data = new ThreadData();
                    data->x = i;
                    data->y = j;
                    data->index = ActiveThreads;

                    thread[ActiveThreads] = SDL_CreateThread(Thread_func, "rtrThread", data);

                    ActiveThreads++;

                }
                else if (ActiveThreads == threads)
                {
                    for (SDL_Thread* var : thread)
                    {
                        SDL_WaitThread(var, &threadReturnValue);
                    }
                    ActiveThreads = 0;
                }
            }
        }
    }
    SDL_Window* window = SDL_CreateWindow("Wi-Fi router location optimizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN); //This is problematic if we release this to the public please remove later thx

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawBlendMode(renderer, lighten);
    ClearRenderer(renderer);
    //Final display
    //Draw optimal routerconfig
    DrawRouter(renderer, (int)OptimalSpot.Location.x, (int)OptimalSpot.Location.y);
    //Draw the walls
    DrawWalls(renderer);
    //Render the above
    DrawRect(renderer, (int)OptimalSpot.Location.x - 3, (int)OptimalSpot.Location.y - 3, 6, 6);

    SDL_RenderPresent(renderer);
    //Print configuration efficiency to the console
    std::cout << OptimalSpot.Coverage << " is  the coverage at the optimal location (" << OptimalSpot.Location.x << ", " << OptimalSpot.Location.y << ")\n";
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


void CalculatePosition(SDL_Renderer* renderer, int i, int j) 
{
    DrawRouter(renderer, i, j);
    //Render the above
    SDL_RenderPresent(renderer);
    CalcCov* calccov = new CalcCov();
    //Calculate the coverage of the current setup
    float Coverage = calccov->CalculateCoverage(renderer);

    std::cout << Coverage << " at " << i << ", " << j << "\n";

    if (Coverage > OptimalSpot.Coverage) {
        RouterConfig currentConfig;
        currentConfig.Coverage = Coverage;
        currentConfig.Location = { (float)i,(float)j };
        OptimalSpot = currentConfig;
    }
    delete calccov;
    ClearRenderer(renderer);
}

void DrawRouter(SDL_Renderer* renderer, int bx, int by) 
{

    for (float phi = 0; phi <= 2.0f * M_PI; phi += accuracy)
    {
        Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), bx, by);
        float colour = ModelData::GetStartStrength();
        SDL_SetRenderDrawColor(renderer, colour, colour, colour, 255);       //Need to do something with these opacities
        colour = DrawLine(renderer, bx, by, e.x, e.y, colour, phi);

        if (ModelData::GetPermittivity(Walls::OnWall(e.x, e.y).walltype) > 0)
            DrawPermittivity(renderer, bx, by, e.x, e.y, colour, phi);
        SDL_RenderPresent(renderer);

    }
}

void DrawPermittivity(SDL_Renderer* renderer, float bx, float by, float ex, float ey, int WifiStrength, float angle)
{
    ModelData::Wall wall = Walls::OnWall(ex, ey);
    float colour = WifiStrength - ModelData::GetPermittivity(wall.walltype);


    SDL_RenderPresent(renderer);

    Vector2 e = Walls::FindFirstCollision(cos(angle), sin(angle), ex, ey);
    if (Walls::OnWall(ex, ey, false))
    {
        ModelData::Wall a = Walls::OnWall(e.x, e.y);
        float WifiStrength2 = DrawLine(renderer, ex, ey, e.x, e.y, colour, angle) - ModelData::GetPermittivity(a.walltype);

        if (WifiStrength2 - ModelData::GetPermittivity(a.walltype) > 1)
            DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2, angle);
    }

}
float DrawLine(SDL_Renderer* renderer, float bx, float by, float ex, float ey, float WifiStrength, float angle) {

    float dif = (-WifiStrength + ceil(WifiStrength));
    SDL_SetRenderDrawColor(renderer, ceil(WifiStrength), ceil(WifiStrength), ceil(WifiStrength), 255);

    float distance = sqrt(pow(bx - ex, 2) + pow(by - ey, 2));
    float remainingdistance = (40 / ModelData::GetLPM() * (dif));

    float rcx = cos(angle);
    float rcy = sin(angle);

    if (remainingdistance < distance)
    {
        SDL_RenderDrawLineF(renderer, bx, by, bx + (rcx * remainingdistance), by + (rcy * remainingdistance));
        WifiStrength = floor(WifiStrength);
        SDL_SetRenderDrawColor(renderer, WifiStrength, WifiStrength, WifiStrength, 255);
    }
    float a = ex;
    float b = ey;
    float colour = ceil(WifiStrength);
    float index = 0;
    for (float i = remainingdistance; i < distance - remainingdistance; i += (40 / ModelData::GetLPM()))
    {
        if (WifiStrength - index * ModelData::GetLPM() > 0)
        {
            colour = WifiStrength - index * ModelData::GetLPM();
            SDL_SetRenderDrawColor(renderer, colour,colour,colour, 255);

            float g = i;
            float g2 = (i + (40 / ModelData::GetLPM()));
            SDL_RenderDrawLineF(renderer, bx + rcx * g, by + rcy * g, bx + rcx * g2, by + rcy * g2);
            a = bx + rcx * g2;
            b = by + rcy * g2;
        }
        else 
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            colour = 0;
        }
        index++;
    }
    SDL_RenderDrawLineF(renderer, a, b, ex, ey);

    if(colour != 0)
        colour += sqrt(pow(a - ex, 2) + pow(b - ey, 2)) / (40 / ModelData::GetLPM());
    return colour;
}

void ClearRenderer(SDL_Renderer* renderer) 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

void DrawWalls(SDL_Renderer* renderer) 
{

    for (int i = 0; i < ModelData::size; i++)
    {
        ModelData::Wall toDraw = ModelData::GetWall(i);

        if(toDraw.walltype == 0)
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        else
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        //Draw the wall to screen
        if (toDraw.x) {
            SDL_RenderDrawLineF(renderer, (int)toDraw.height, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine);
        }

        if (!toDraw.x)
        {
            SDL_RenderDrawLineF(renderer, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine, (int)toDraw.height);
        }

    }
}

void DrawRoom(SDL_Renderer* renderer,int index, int intensity) {

    SDL_Rect* a = new SDL_Rect; 
    a->x = ModelData::GetRoom(index).lc.x;
    a->y = ModelData::GetRoom(index).lc.y;
    a->w = ModelData::GetRoom(index).rc.x - ModelData::GetRoom(index).lc.x;
    a->h = ModelData::GetRoom(index).rc.y - ModelData::GetRoom(index).lc.y;

    intensity = intensity / ((a->x - a->w) * (a->y - a->h));
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, intensity);

    SDL_RenderFillRect(renderer, a);
}
void DrawRect(SDL_Renderer* renderer, int x1, int y1, int x2, int y2) {

    SDL_Rect* a = new SDL_Rect;
    a->x = x1;
    a->y = y1;
    a->w = x2;
    a->h = y2;

    SDL_RenderFillRect(renderer, a);
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
