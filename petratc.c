#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "rat.h"
#include "defs.h"

#ifdef __APPLE__
  #define ratpath  "%s/Library/Application Support/rat"
  #define ratsave "%s/Library/Application Support/rat/rat.txt"
#else
  #define ratpath  "%s/.local/share/rat"
  #define ratsave "%s/.local/share/rat/rat.txt"
#endif

  char ratart[] = 
        "     .---.\n  (\\./)     \\.......-\n  >' '<  (__.'\"\"\"\"\"\"\n  \" ` \" \"_";

  char *messages[] = {
    "squeak!",
    "squeak! !",
    "squeak",
    "squeak...",
    "SQUEAK",
    "SQUEAK!!",
    "SQUEAAAKKKKK",
    };

void addstat(float *guy){

    if(*guy < 98.5){
        *guy += 1.5;
    } else{*guy += 0.05;}     
}

Rat rat;

void save(void) {
    char path[256];
    snprintf(path, sizeof(path), ratpath, getenv("HOME"));
    mkdir(path, 0755);
    snprintf(path, sizeof(path), ratsave, getenv("HOME"));

    FILE *f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "%f\n%f\n%f\n%f\n%f\n%d\n",
        rat.hunger, rat.love, rat.fun, rat.clean, rat.health, rat.age);
    fclose(f);
}

void load(void) {
    char path[256];
    snprintf(path, sizeof(path), ratsave, getenv("HOME"));

    FILE *f = fopen(path, "r");
    if (!f) {
        rat.hunger = 100.0f;
        rat.love   = 100.0f;
        rat.fun    = 100.0f;
        rat.clean  = 100.0f;
        rat.health = 100.0f;
        rat.age    = time(NULL);
        return;
    }
    fscanf(f, "%f\n%f\n%f\n%f\n%f\n%d\n",
        &rat.hunger, &rat.love, &rat.fun, &rat.clean, &rat.health, &rat.age);
    fclose(f);
}

float decay(float current, int timepassed, float rate) {
    float result = current - (timepassed * rate);
    return result < 0.0f ? 0.0f : result;
}

void setstat(){
    int timenow = time(NULL);
    int newage = timenow - rat.age;  

    rat.hunger = decay(rat.hunger, newage, 0.005f);
    rat.love   = decay(rat.love, newage, 0.002f);
    rat.fun    = decay(rat.fun, newage, 0.006f);
    rat.clean  = decay(rat.clean, newage, 0.0002f);
    rat.health = decay(rat.health, newage, 0.00002f);
    

    rat.age = timenow; //thinking about this makes my cortisol spike so i'll just ignore it even more
}   
    

void printrat(){

    printf("%s\n", ratart);
    int howmany = sizeof(messages) / sizeof(messages[0]);
    printf("%s\n", messages[rand() % howmany]);
    printf("rat hunger: %.2f, rat love: %.2f, rat fun: %.2f, rat clean: %.2f, rat health: %.2f\n",
            rat.hunger, rat.love, rat.fun, rat.clean, rat.health);
 
}


int main(){
srand(time(NULL));
int choice;
int truing = 1;

    load();
    setstat();
    printrat();

    while(truing){
    printf("1: play a game! (fun up), 2: go down to the maze... (hunger up), 4: take a shower! (clean up), 9: print me!, 0: exit...\n");
    
    if(scanf("%d", &choice) != 1){
        while(getchar() != '\n');  //flush the bad input if you're a bad boy and put q instead of 1 
        continue;
    }    
    switch(choice){

        case 1: gameguess(); break;
        case 2: gamemaze(); break; 
        case 4: gamecatch(); break; 
        case 9: printrat(); break;
        case 0: truing = 0; break; 
        }
    }


    save();
    printf("bye bye~~\n");
    return(0);
}
