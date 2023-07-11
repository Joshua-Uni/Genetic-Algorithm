#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>


//
//CONSTANTS:
//
#define DNA_LENGTH 300 //MUST BE A CONSTANT TO BE PUT IN A STRUCT
#define FLOCK_SIZE 750
#define WIDTH 960
#define HEIGHT 720
#define MAX_VEl 5
#define FPS 30
#define CREATURE_WIDTH 5
#define CREATURE_HEIGHT 5
#define VARIATION_RATE 0.03 //%of genes chance (between 0 and 1)


//
//GLOBAL VARS:
//
SDL_Surface *windowSurface = NULL;
int START_X;
int START_Y;


//
//STRUCTURES:
//
typedef struct Creature {
    //STRUCT FOR THE DOT aka "CREATURE"
    //Movement
    int DNA_x[DNA_LENGTH];
    int DNA_y[DNA_LENGTH];
    int x_vel;
    int y_vel;
    int max_vel;
    //Colour
    Uint32 colour;
    int r;
    int g;
    int b;
    //Survival Score
    bool alive;
    float survival_score;
    bool hit_target;
    int hit_target_index; //index at which creature hit the target (or 0 if it didn't)
    bool hit_wall;
    //Body
    SDL_Rect rect; //The body
    int start_x;
    int start_y;
} creature;

typedef struct Target {
    SDL_Rect rect;
    Uint32 colour;
} target;

typedef struct Wall {
    Uint32 colour;
    SDL_Rect rect;

} wall;


//
//FUNCTIONS:
//
creature createCreature();
void moveCreature(creature *creature_pointer, int i);
void createFirstFlock(creature *flock);
void moveFlock(creature *flock, int i);
void checkAlive(creature *flock,target *target_pointer,wall *wall_pointer, int current_index);
void drawTarget(target *Target_pointer);
void calcSurvivalScores(creature *flock, target *target_pointer, float *totalSurvivalScore);
void makeNewFlock(creature *oldFlock, creature *newFlock ,float *totalSurvivalScoreP);
int newColour(int colour);
int newSize(int size);
int newStart(int start, bool is_x);
void drawWalls(wall *walls_p);
void makeWalls(wall *walls_p);


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





//
//AUXILLIARY:
//
creature createCreature() {
    //Creates a creature. This is only used for the FIRST FLOCK. Subsequent flocks are made using the makeNewFlock function.
    //Size, DNA and colour are randomly generated
    //Returns the creature

    creature current;
    int random;
    int random_power;

    //Set starting stats
    current.start_x = START_X;
    current.start_y = START_Y;
    current.rect.x =  current.start_x;
    current.rect.y = current.start_y;
    current.x_vel = 0;
    current.y_vel = -2;
    current.max_vel = MAX_VEl;
    current.alive = true;
    current.survival_score = 0.0;
    current.hit_target = false;
    current.hit_target_index = 0;
    current.hit_wall = false;

    //INITALISE SIZE
    current.rect.h = newSize(CREATURE_WIDTH);
    current.rect.w = current.rect.h;

    //INITALISE CREATURE COLOUR
    int r_rand = rand() % 156; // 0 to 155
    current.r = r_rand + 100; //100 to 255
    int g_rand = rand() % 156; // 0 to 155
    current.g = g_rand + 100; //100 to 255
    int b_rand = rand() % 156; // 0 to 155
    current.b = b_rand + 100; //100 to 255

    current.colour = SDL_MapRGB(windowSurface->format,current.r,current.g,current.b);

    //INITALISE CREATURE DNA RANDOMLY
    //random is either -1,0,1 (the direction)
    //random_power is multiplied (the strength of direction)
    for (int i=0;i<DNA_LENGTH;i++){
        //Set x
        random = rand() % 3; // 0||1||2
        random_power = rand() % 5;
        if (random == 2)
        {
            random = -1;
        }
        current.DNA_x[i] = random * random_power;

        //Set y
        random = rand() % 3; // 0||1||2
        random_power = (rand() % 3)+1; //1 to 3
        if (random == 2)
        {
            random = -1;
        }
        current.DNA_y[i] = random*random_power;
    }

    return current;
}



void createFirstFlock(creature *flock){
    //Using createCreature() function create a unique creature for each room in flock array
    for (int i=0;i<FLOCK_SIZE;i++)
    {
        flock[i] = createCreature();
    }
}



void moveCreature(creature *creature_pointer, int i) {
    //Moves creature taking into account its current DNA and velocity and draw it
    //DNA adds to velocity. Velocity adds to x/y position. Makes for smoother movement since velocity persists over time.
    //If creature has run out of DNA it dies. If dead it doesnt move.

    //CHECK IF THERE'S DNA
    if (i >= DNA_LENGTH)
    {
        creature_pointer->alive = false;
    }

    //IF ALIVE:
    if (creature_pointer->alive == true)
    {
        //UPDATE VELOCITY
        creature_pointer->x_vel += creature_pointer->DNA_x[i];
        creature_pointer->y_vel += creature_pointer->DNA_y[i];

        //KEEP VELOCITY UNDER MAX_VEL
        if (creature_pointer->x_vel > creature_pointer->max_vel)
        {
            creature_pointer->x_vel = creature_pointer->max_vel;
        }
        else if (creature_pointer->x_vel < -1*creature_pointer->max_vel)
        {
            creature_pointer->x_vel = -1*creature_pointer->max_vel;
        }
        if (creature_pointer->y_vel > creature_pointer->max_vel)
        {
            creature_pointer->y_vel = creature_pointer->max_vel;
        }
        else if (creature_pointer->y_vel < -1*creature_pointer->max_vel)
        {
            creature_pointer->y_vel = -1*creature_pointer->max_vel;
        }

        //UPDATE XY
        creature_pointer->rect.x += creature_pointer->x_vel;
        creature_pointer->rect.y += creature_pointer->y_vel;
    }

    //DRAW CREATURE
    SDL_FillRect(windowSurface, &(creature_pointer->rect), creature_pointer->colour);
}



void moveFlock(creature *flock,int i)
{
    //Uses moveCreature() function on each member of the flock

    for (int creatureIndex=0;creatureIndex<FLOCK_SIZE;creatureIndex++)
    {
        moveCreature(&(flock[creatureIndex]),i);
    }

}



void checkAlive(creature *flock,target *target_pointer,wall *wall_pointer, int current_index)
{
    //set .alive status to false for a creatur if it hits the edges of the screen, hits the target or hits the wall.
    //set colour of dead creatures to red
    //special status's if you hit a wall (negative) or target (positive)

    creature *current;
    for (int creatureIndex=0;creatureIndex<FLOCK_SIZE;creatureIndex++)
    {
        //NOTE THAT IT HAS TO BE AN ADDRESS. If its just flock[i] it must make a copy and then is forgotten/resets after the function is over. Getting an address is how we access memeory outside a function.
        current = &(flock[creatureIndex]);

        //OUT OF BOUNDS
        if ((current->rect.x<0) || (current->rect.x + CREATURE_WIDTH > WIDTH) || (current->rect.y<0) || (current->rect.y+CREATURE_HEIGHT > HEIGHT))
        {  
            current->colour = SDL_MapRGB(windowSurface->format,255,0,0);
            current->alive = false;
        } 

        //HIT TARGET
        if ((current->rect.x > target_pointer->rect.x) & (current->rect.x < target_pointer->rect.x+target_pointer->rect.w) & (current->rect.y < target_pointer->rect.y+target_pointer->rect.h))
        {
            current->colour = SDL_MapRGB(windowSurface->format,255,0,0);
            current->hit_target = true;
            current->hit_target_index = current_index;
            current->alive = false;
        }

        //HIT WALL
        bool inside_x = (current->rect.x < wall_pointer->rect.x+wall_pointer->rect.w)&(current->rect.x+current->rect.w > wall_pointer->rect.x); //True if creature is in the x range of wall
        bool fromTop = (current->rect.y < wall_pointer->rect.y+wall_pointer->rect.h) & (current->rect.y + current->rect.h > wall_pointer->rect.y) & (inside_x);
        if (fromTop)
        {
            current->colour = SDL_MapRGB(windowSurface->format,255,0,0);
            current->alive = false;  
            current->hit_wall = true;
        }
    }      
}



void drawTarget(target *target_pointer)
{
    SDL_FillRect(windowSurface, &(target_pointer->rect), target_pointer->colour);
}



void calcSurvivalScores(creature *flock, target *target_pointer, float *totalSurvivalScoreP)
{
    //Higher survival scores means that the creature has a higher chance of getting into the next generation
    //Closer creature is to target the higher the survival score
    //Bonus for hitting the target (and bonus for hitting the target earlier than other creatures)
    //Penalty for hitting the barrier

    creature *current;
    int target_x = target_pointer->rect.x + floor(target_pointer->rect.w/2);
    int target_y = target_pointer->rect.y + floor(target_pointer->rect.h/2);
    int creature_x;
    int creature_y;
    float distance;

    for (int creatureIndex=0;creatureIndex<FLOCK_SIZE;creatureIndex++)
    {
        current = &(flock[creatureIndex]);

        //middle of target and creature
        creature_x = current->rect.x + floor(current->rect.w/2);
        creature_y = current->rect.y + floor(current->rect.h/2);

        //CALCULATE SURVIVAL SCORE
        distance = (target_x-creature_x)*(target_x-creature_x) + (target_y-creature_y)*(target_y-creature_y);
        current->survival_score = (1/distance)*100000;
        if (current->hit_target==true)
        {
            current->survival_score += (DNA_LENGTH - current->hit_target_index)*100;
        }
        if (current->hit_wall==true)
        {
            current->survival_score = current->survival_score/10;
        }
        *totalSurvivalScoreP += current->survival_score; //Used in calculating chance of getting into new generation.
    }
}



void makeNewFlock(creature *oldFlock, creature *newFlock ,float *totalSurvivalScoreP)
{
    //Populate newFlock by using creatures from oldFlock
    //Firstly: iterate over oldFlock creatures with chance (higher if survival score higher) creature is copied from oldFlock to newFlock
    //Secondly: once newFlock is populated, randomly mutate the DNA, colour and and size of creatures in newFlock to add genetic diversity


    int oldCreatureIndex = 0;
    float creatureOdds; //odds higher when survival score higher (0-1)
    float chance; //random number between 0-1

    
    for (int newCreatureIndex=0; newCreatureIndex<FLOCK_SIZE; newCreatureIndex++) //Iterate over new creatures
    {   
        //FIRST: POPULATE NEWFLOCK
        while (true)
        {        
            //Iterate over old flock. if creatureOdds higher than chance creature moved to newFlock
            creatureOdds = (oldFlock[oldCreatureIndex].survival_score) / *totalSurvivalScoreP; //0-1
            chance = (float)rand()/(float)(RAND_MAX);

            if (creatureOdds>chance)
            {   
                //Copy old creature to newFlock and reset some of its statistics
                memcpy((&newFlock[newCreatureIndex]),&(oldFlock[oldCreatureIndex]), sizeof(oldFlock[oldCreatureIndex]));
                newFlock[newCreatureIndex].x_vel = 0;
                newFlock[newCreatureIndex].y_vel = -2;
                newFlock[newCreatureIndex].alive = true;
                newFlock[newCreatureIndex].survival_score = 0.0;
                newFlock[newCreatureIndex].hit_target = false;
                newFlock[newCreatureIndex].hit_target_index = 0;
                newFlock[newCreatureIndex].hit_wall = false;

                break;
            }
            else
            {   
                //Try odds with next creature from oldFlock
                oldCreatureIndex += 1;
                if (oldCreatureIndex >= FLOCK_SIZE)
                {
                    oldCreatureIndex = 0;
                }
            }

        }
        

        //SECOND: ADD GENETIC VARIABILLITY
        float DNA_Odds;
        int random;
        int random_power;

        //DNA_x
        for (int DNA=0;DNA<DNA_LENGTH;DNA++)
        {
            DNA_Odds = (float)rand()/(float)(RAND_MAX); //Between 0-1
            if (DNA_Odds>(1-VARIATION_RATE))
            {
                //make a new DNA point
                random = rand() % 3; // 0||1||2
                random_power = rand() % 5+1;
                if (random == 2)
                {
                    random = -1;
                }
                newFlock[newCreatureIndex].DNA_x[DNA] = random * random_power;
            }
        }

        //DNA_y
        for (int DNA=0;DNA<DNA_LENGTH;DNA++)
        {
            DNA_Odds = (float)rand()/(float)(RAND_MAX); //Between 0-1
            if (DNA_Odds>(1-VARIATION_RATE))
            {
                //make a new DNA point
                random = rand() % 3; // 0||1||2
                random_power = rand() % 5+1;
                if (random == 2)
                {
                    random = -1;
                }
                newFlock[newCreatureIndex].DNA_y[DNA] = random * random_power;
            }
        }

        //Colour
        newFlock[newCreatureIndex].r = newColour(newFlock[newCreatureIndex].r);
        newFlock[newCreatureIndex].g = newColour(newFlock[newCreatureIndex].g);
        newFlock[newCreatureIndex].b = newColour(newFlock[newCreatureIndex].b);
        newFlock[newCreatureIndex].colour = SDL_MapRGB(windowSurface->format,newFlock[newCreatureIndex].r,newFlock[newCreatureIndex].g,newFlock[newCreatureIndex].b);

        //Size
        newFlock[newCreatureIndex].rect.w = newSize(newFlock[newCreatureIndex].rect.w);
        newFlock[newCreatureIndex].rect.h = newFlock[newCreatureIndex].rect.w;

        //Start Pos
        newFlock[newCreatureIndex].start_x = newStart(newFlock[newCreatureIndex].start_x,true);
        newFlock[newCreatureIndex].start_y = newStart(newFlock[newCreatureIndex].start_y,false);
        newFlock[newCreatureIndex].rect.x = newFlock[newCreatureIndex].start_x;
        newFlock[newCreatureIndex].rect.y = newFlock[newCreatureIndex].start_y;
    }
}



int newColour(int colour)
{
    //Given a rgb value, slightly alter it then return

    int random = rand() % 2; // 0||1
    int random_power = rand()%20 +1; 
    if (random == 0)
    {
        random = -1;
    }
    colour += random * random_power;

    //Boundaries
    if (colour > 240)
    {
        colour = 240;
    }
    else if (colour < 50)
    {
        colour = 50;
    }
    return colour;
}



int newSize(int size)
{
    //Given a size value, slightly alter it then return
    //Has a high liklihood of not changing

    int max_random = 5;
    int random = rand() % max_random; // 0||1||2||3||4
    int random_power = rand()%2; 

    //0,1,2 --> no change in size
    //3 --> increase
    //4 --> decrease
    if (random < max_random-2)
    {
        random = 0;
    } else if (random == max_random-2)
    {
        random = 1;
    }
    else
    {
        random = -1;
    }
    size += random * random_power;

    //constrict size to a limit
    if (size > CREATURE_WIDTH+2)
    {
        size = CREATURE_WIDTH+2;
    }
    else if (size < CREATURE_WIDTH-2)
    {
        size = CREATURE_WIDTH-2;
    }
    return size;
}



int newStart(int start, bool is_x)
{
    //Given a start value, slightly alter it then return
    //differentiates between x and y start values

    int random = rand() % 3;
    int random_power = rand()%15+1; 

    if (random == 2)
    {
        random = -1;
    }
    start += random * random_power;

    //constrict size to a limit
    if (is_x) //x start
    {
        if (start > START_X+floor(WIDTH/3))
        {
            start = START_X+floor(WIDTH/3);
        }
        else if (start < START_X-floor(WIDTH/3))
        {
            start = START_X-floor(WIDTH/3);
        }
    }
    else //y start
    {
        if (start > START_Y+floor(HEIGHT/8))
        {
            start = START_Y+floor(HEIGHT/8);
        }
        else if (start < START_Y-floor(HEIGHT/8))
        {
            start = START_Y-floor(HEIGHT/8);
        }
    }
    return start;
}



void makeWalls(wall *walls_p)
{
    //Set parameters for wall

    walls_p->rect.x = floor(WIDTH*3/8);
    walls_p->rect.y = floor(WIDTH/2-WIDTH/5);
    walls_p->rect.w = (WIDTH*2/8);
    walls_p->rect.h = 30;
    walls_p->colour = SDL_MapRGB(windowSurface->format,200,50,50);;
}



void drawWalls(wall *walls_p)
{
    SDL_FillRect(windowSurface, &(walls_p->rect), walls_p->colour);
}