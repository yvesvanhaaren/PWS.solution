#include "SDL.h"
#include "Walls.h"
#include "Vectors.h"
#include "ModelData.h"
#include <string>

void DrawReflection(SDL_Renderer* renderer, int bx, int by, int ex, int ey, int WifiStrength);
void DrawPermittivity(SDL_Renderer* renderer, int bx, int by, int ex, int ey, int WifiStrength);
void DrawRoom(SDL_Renderer* renderer, int index, int intensity);
void DrawRect(SDL_Renderer* renderer, int x1, int y1, int x2, int y2);
void SaveScreenshot(SDL_Renderer* renderer, std::string name);
float CalculateCoverage(SDL_Renderer* renderer);
void DrawRouter(SDL_Renderer* renderer, int bx, int by);
void DrawWalls(SDL_Renderer* renderer);
void ClearRenderer(SDL_Renderer* renderer);
SDL_Color GetPixel(SDL_Surface* srf, int x, int y);
Uint32 get_pixel32(SDL_Surface* surface, int x, int y);

const int width = ModelData::width;
const int height = ModelData::height;
const float accuracy = (2 * M_PI) / (18);
int rtrDistance = ModelData::GetrtrDistance();


struct RouterConfig {
    Vector2 Location;
    float Coverage;
};
int main(int argc, char* argv[])
{

    //Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window;
    //window = SDL_CreateWindow("Wi-Fi router location optimizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    
    window = SDL_CreateWindow("Wi-Fi router location optimizer", -1200, 50, width, height, SDL_WINDOW_SHOWN); //This is problematic if we release this to the public please remove later thx
    

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    ClearRenderer(renderer);
    
    void Reflection(SDL_Renderer * renderer, int index, int intensity);
    //Simulate Wi-Fi router
    RouterConfig OptimalSpot = { {0,0}, 0 };
    for (int i = 0; i < width; i += rtrDistance)
    {
        for (int j = 0; j < height; j += rtrDistance)
        {
            if (!Walls::OnWall(i, j, true))
            {
                DrawRouter(renderer, i, j);
                //Render the above
                SDL_RenderPresent(renderer);

                //Calculate the coverage of the current setup
                float Coverage = CalculateCoverage(renderer);

                DrawWalls(renderer);
                SDL_RenderPresent(renderer);    //Could be removed to enhance performance

                std::cout << Coverage << " at " << i << ", " << j << "\n";

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
            else
            {
                std::cout << "Location is obstructed\n";
            }
        }
    }
    //Final display
    //Draw optimal routerconfig
    DrawRouter(renderer, (int)OptimalSpot.Location.x, (int)OptimalSpot.Location.y);
    //Draw the walls
    DrawWalls(renderer);
    //Render the above
    DrawRect(renderer, (int)OptimalSpot.Location.x - 3, (int)OptimalSpot.Location.y - 3,6,6);

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
    for (float phi = 0; phi <= 1.99 * M_PI; phi += accuracy)
    {
        Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), bx, by);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);       //Need to do something with these opacities
        SDL_RenderDrawLine(renderer, bx, by, e.x, e.y);

        DrawReflection(renderer, bx, by, e.x, e.y, 255);
        DrawPermittivity(renderer, bx, by, e.x, e.y, 255);
    }
}

void DrawReflection(SDL_Renderer* renderer, int bx, int by, int ex, int ey, int WifiStrength) 
{
    ModelData::Wall wall = Walls::OnWall(ex, ey);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype)); 
    float WifiStrength2 = (float)WifiStrength * ModelData::GetReflection(wall.walltype);
    if (wall.x)
    {
        if (wall.height > bx) 
        {
            for (float phi = 0.5 * M_PI + accuracy; phi <= 1.49 * M_PI; phi += accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false))
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 30)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype)); 
                    }
                }
            }
        }
        else if (wall.height < bx) 
        {
            for (float phi = 0.5 * M_PI - accuracy; phi >= -0.49 * M_PI; phi -= accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false)) 
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 30)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype));  
                    }
                }
            }
        }

    }
    else if (!wall.x) 
    {
        if (wall.height > by) //no longer creates circles
        {
            for (float phi = (M_PI) + accuracy; phi <= (1.99 * M_PI); phi += accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false))
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 30)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype));
                    }
                }
            }
        }
        else if (wall.height < by) 
        {
            for (float phi = M_PI - accuracy; phi >= 0.01; phi -= accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false))
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 30)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype));
                    }
                }
            }
        }
    }

}
void DrawPermittivity(SDL_Renderer* renderer, int bx, int by, int ex, int ey, int WifiStrength)
{
    ModelData::Wall wall = Walls::OnWall(ex, ey);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetPermittivity(wall.walltype));  //Need to do something with these opacities
    int WifiStrength2 = WifiStrength * ModelData::GetPermittivity(wall.walltype);

    if (wall.x)
    {
        if (wall.height < bx)
        {
            for (float phi = 0.5 * M_PI + accuracy; phi <= 1.49 * M_PI; phi += accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false)) 
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 1)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype));
                    }
                }
            }
        }
        else if (wall.height > bx)
        {
            for (float phi = 0.5 * M_PI - accuracy; phi >= -0.49 * M_PI; phi -= accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false)) 
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 1)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype));
                    }
                }

            }
        }

    }
    else if (!wall.x)
    {
        if (wall.height < by) //no longer creates circles
        {
            for (float phi = (M_PI)+accuracy; phi <= (1.99 * M_PI); phi += accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false)) 
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 1)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype));
                    }
                }
            }
        }
        else if (wall.height > by)
        {
            for (float phi = M_PI - accuracy; phi >= 0.01; phi -= accuracy)
            {
                Vector2 e = Walls::FindFirstCollision(cos(phi), sin(phi), ex, ey);
                if (Walls::OnWall(ex, ey, false)) 
                {
                    SDL_RenderDrawLine(renderer, ex, ey, e.x, e.y);
                    if (WifiStrength2 > 1)
                    {
                        DrawReflection(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        DrawPermittivity(renderer, ex, ey, e.x, e.y, WifiStrength2);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, WifiStrength * ModelData::GetReflection(wall.walltype));
                    }
                }
            }
        }
    }

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
            SDL_RenderDrawLine(renderer, (int)toDraw.height, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine);
        }

        if (!toDraw.x)
        {
            SDL_RenderDrawLine(renderer, (int)toDraw.domains, (int)toDraw.height, (int)toDraw.domaine, (int)toDraw.height);
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

float* coverage = new float[width * height]; // * allocates memory
SDL_Surface* coveragesurface;

float CalculateCoverage(SDL_Renderer* renderer) 
{
    //Calculate coverage
    const Uint32 format = SDL_PIXELFORMAT_ARGB8888;
    coveragesurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format); // * -> heap || no * -> stack
    SDL_RenderReadPixels(renderer, NULL, format, coveragesurface->pixels, coveragesurface->pitch);


    coverage[0] = 1;

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            SDL_Color col;
            col = GetPixel(coveragesurface, i, j);
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

    

    SDL_FreeSurface(coveragesurface);
    return omega;
    //Surface and coverage are never deleted so program uses considerably more memory than necessary
}