#include "aux.h"

SDL_Surface *windowSurface = NULL;

//
//MAIN:
//
int main()
{
    //INITALISE SDL + MAKE A WINDOW:
    SDL_Window *window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO)<0)
    {
        printf("Video Initialisation Error");
    }
    else
    {
        window = SDL_CreateWindow("Genetic Algorithm",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window Creation Error");
        }
        else
        {
            windowSurface = SDL_GetWindowSurface(window);

            SDL_UpdateWindowSurface(window);
            SDL_PumpEvents(); //NEEDED IN MAC
        }
    }

    
    //PARAMETERS:
    //Color
    Uint32 white = SDL_MapRGB(windowSurface->format,0xff,0xff,0xff);
    Uint32 black = SDL_MapRGB(windowSurface->format,0x00,0x00,0x00);
    //Creature/Flock
    int creatureIndex = 0;
    float totalSurvivalScore = 0;
    START_X = floor(WIDTH/2);
    START_Y = HEIGHT-floor(HEIGHT/10);
    creature flock[FLOCK_SIZE];
    //Time
    int prevTime = 0;
    int currentTime = 0;
    float frameTime = 0;
    float deltaTime = 0;
    //Random
    time_t t;
    srand((unsigned) time(&t)); //create a random seed for the rand() function

    //MAKE TARGET
    target Target;
    Target.rect.w = 100;
    Target.rect.h = 35;
    Target.rect.x = floor(WIDTH/2)-floor(Target.rect.w/2);
    Target.rect.y = 0;
    Target.colour = SDL_MapRGB(windowSurface->format,212,175,55);

    //MAKE FIRST FLOCK:
    createFirstFlock(flock);

    //MAKE WALL
    wall walls;
    makeWalls(&walls);


    //
    //MAIN GAME LOOP:
    //
    SDL_Event ev; //store events
    bool isRunning = true;

    while(isRunning)
    {   
        //TIME:
        prevTime = currentTime;
        currentTime = SDL_GetTicks(); //Get ticks = amount of milliseconds since application started
        deltaTime = currentTime-prevTime; //milliseconds since last game loop
        deltaTime = deltaTime / 1000.0f; //seconds since last gameloop
        frameTime += deltaTime;

        if (frameTime < (1.0/FPS)) {
            continue;
        }
        frameTime = 0;

        //EVENT LOOP:
        while(SDL_PollEvent(&ev)!=0)
        {
            //EXIT BUTTON
            if(ev.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        //DRAW BLACK SCREEN:
        SDL_FillRect(windowSurface, NULL, black);

        //DRAW TARGET
        drawTarget(&Target);

        //DRAW WALL
        drawWalls(&walls);

        //MOVE CREATURE:
        moveFlock(flock,creatureIndex);
        creatureIndex += 1;

        //CHECK IF FLOCK ALIVE:
        checkAlive(flock, &Target,&walls,creatureIndex);

        //CHECK FOR END OF FLOCK LIFESPAN
        if (creatureIndex >= DNA_LENGTH)
        {
            //MAKE NEW FLOCK
            creatureIndex = 0;
            calcSurvivalScores(flock, &Target,&totalSurvivalScore);
            creature newFlock[FLOCK_SIZE];
            makeNewFlock(flock,newFlock,&totalSurvivalScore);
            memcpy(&flock,&newFlock,sizeof(flock));
            totalSurvivalScore = 0;
        }

        //UPDATE WINDOW
        SDL_UpdateWindowSurface(window);
        SDL_PumpEvents(); //NEEDED IN MAC

    }
    //END OF MAIN GAME LOOP: EXIT
    SDL_FreeSurface(windowSurface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


