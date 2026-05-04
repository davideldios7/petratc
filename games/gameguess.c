#include <stdio.h>
#include <stdlib.h>
#include "../rat.h"

void gameguess(){
int guess;
int solved = 0;
int range = 100;

int correct = (rand() % (range + 1));

printf("let's play a game, squeak!~ you gues a number between 0 and %d!\n", range);


while(solved != 1){

if(scanf("%d", &guess) != 1){
    while(getchar() != '\n');  //flush the bad input here too
    continue;
} 

if(guess < correct){
printf("number too low try again\n");
}else if(guess > correct){
printf("number too high try again\n");
}else {printf("yes!! correct! squeaaakk!~\n"); solved = 1;}
}

    addstat(&rat.fun);

}
