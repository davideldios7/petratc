#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "rat.h"

void save(void) {
    char path[256];
#ifdef __APPLE__
    snprintf(path, sizeof(path), "%s/Library/Application Support/rat", getenv("HOME"));
#else
    snprintf(path, sizeof(path), "%s/.local/share/rat", getenv("HOME"));
#endif 
    mkdir(path, 0755);
#ifdef __APPLE__
    snprintf(path, sizeof(path), "%s/Library/Application Support/rat/rat.txt", getenv("HOME"));
#else    
    snprintf(path, sizeof(path), "%s/.local/share/rat/rat.txt", getenv("HOME")); 
#endif

    FILE *f = fopen(path, "w");
    if (!f) return;

    fprintf(f, "%f\n%f\n%f\n%f\n%f\n%d\n",
        rat.hunger, rat.love, rat.fun,
        rat.clean, rat.health, rat.age);
    fclose(f);
}

void load(void) {
    char path[256];
#ifdef __APPLE__
    snprintf(path, sizeof(path), "%s/Library/Application Support/rat/rat.txt", getenv("HOME"));
#else
    snprintf(path, sizeof(path), "%s/.local/share/rat/rat.txt", getenv("HOME"));
#endif 
    FILE *f = fopen(path, "r");
    if (!f) {
        //first run
        rat.hunger = 100.0f;
        rat.love   = 100.0f;
        rat.fun    = 100.0f;
        rat.clean  = 100.0f;
        rat.health = 100.0f;
        rat.age    = time(NULL);
        return;
    }

    fscanf(f, "%f\n%f\n%f\n%f\n%f\n%d\n",
        &rat.hunger, &rat.love, &rat.fun,
        &rat.clean, &rat.health, &rat.age);
    fclose(f);
}

float decay(float current, int timepassed, float rate) {
    float result = current - (timepassed * rate);
    return result < 0.0f ? 0.0f : result;
}

void setstat(){
    //int n = rand() % 100; 
    int timenow = time(NULL);
    int newage = timenow - rat.age;  

    rat.hunger = decay(100.0f, newage, 0.005f);
    rat.love   = decay(100.0f, newage, 0.002f);
    rat.fun    = decay(100.0f, newage, 0.006f);
    rat.clean  = decay(100.0f, newage, 0.0002f);
    rat.health = decay(100.0f, newage, 0.00002f);
    

    rat.age = timenow; //while asking grok if the ifdefs for the mac path where fine he said something about time() returning a 64 int in mac but i don't know what that means so i'll ignore it 
}

void printrat(){

    printf("%s\n", ratart);
    int howmany = sizeof(messages) / sizeof(messages[0]);
    printf("%s\n", messages[rand() % howmany]);
    printf("rat hunger: %.2f, rat love: %.2f, rat fun: %.2f, rat dirt: %.2f, rat health: %.2f\n", rat. hunger, rat.love, rat.fun, rat.clean, rat.health);
 
}

void gameguess(){
	int guess;
	int solved = 0;
	int range = 100;

	int correct = (rand() % (range + 1));

	printf("let's play a game, squeak!~ you gues a number between 0 and %d!\n", range);


	while(solved != 1){

		scanf("%d", &guess);
		
		if(guess < correct){
			printf("number too low try again\n");
		}else if(guess > correct){
			printf("number too high try again\n");
		}else {printf("yes!! correct! squeaaakk!~\n"); solved = 1;}
	}


    if(rat.fun < 98.5){rat.fun += 1.5;} else {rat.fun += 0.05;}     //stupid function but it's smart if you think about it. just ignore it 

}

int main(){
srand(time(NULL));
int choice;
int truing = 1;

    load();
    setstat();
    printrat();

    while(truing){
    printf("1: play a game! (fun up), 9: print me!, 0: exit...\n");
    scanf("%d", &choice);
    switch(choice){

        case 1: gameguess(); break;
        case 9: printrat(); break;
        case 0: truing = 0; break; 
        }
    }


    save();
    printf("bye bye~~\n");
    return(0);
}
