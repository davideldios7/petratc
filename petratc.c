#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "rat.h"

#ifdef __APPLE__
  #define ratpath  "%s/Library/Application Support/rat"
  #define ratsave "%s/Library/Application Support/rat/rat.txt"
#else
  #define ratpath  "%s/.local/share/rat"
  #define ratsave "%s/.local/share/rat/rat.txt"
#endif

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
    
//stupid function but it's smart if you think about it. just ignore it 
void addstat(float *guy){

    if(*guy < 98.5){
        *guy += 1.5;
    } else{*guy += 0.05;}     
}

void printrat(){

    printf("%s\n", ratart);
    int howmany = sizeof(messages) / sizeof(messages[0]);
    printf("%s\n", messages[rand() % howmany]);
    printf("rat hunger: %.2f, rat love: %.2f, rat fun: %.2f, rat dirt: %.2f, rat health: %.2f\n",
            rat.hunger, rat.love, rat.fun, rat.clean, rat.health);
 
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

    addstat(&rat.fun);

}

void showergame(){

//ignore this i'm working on it



    addstat(&rat.clean);
}



int main(){
srand(time(NULL));
int choice;
int truing = 1;

    load();
    setstat();
    printrat();

    while(truing){
    printf("1: play a game! (fun up), 4: take a shower!, 9: print me!, 0: exit...\n");
    scanf("%d", &choice);
    switch(choice){

        case 1: gameguess(); break;
        case 4: showergame(); break; 
        case 8: printf("%d\n", rat.age); break;
        case 9: printrat(); break;
        case 0: truing = 0; break; 
        }
    }


    save();
    printf("bye bye~~\n");
    return(0);
}
