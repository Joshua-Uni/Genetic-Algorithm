#ifndef AUX_H
#define AUX_H

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
int START_X;
int START_Y;
extern SDL_Surface *windowSurface;


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
//FUNCTIONS DEFINITIONS:
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

#endif
