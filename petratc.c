#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <sys/stat.h>
#include "rat.h"

#ifdef __APPLE__
  #define ratpath  "%s/Library/Application Support/rat"
  #define ratsave "%s/Library/Application Support/rat/rat.txt"
#else
  #define ratpath  "%s/.local/share/rat"
  #define ratsave "%s/.local/share/rat/rat.txt"
#endif

    char *ratart[] = { 
        "     .---.\n  (\\./)     \\.......-\n  >' '<  (__.'\"\"\"\"\"\"\n  \" ` \" \"_",
        "           .---.\n-......./     (\\./)\n \"\"\"\"\"\"'.__)  >' '<\n           _\" \" ` \"",
        "     .---.\n  (\\./)     \\.......-\n  >= =<  (__.'\"\"\"\"\"\"\n  \" ` \" \"_",
        "     .---.\n  (\\./)     \\.......-\n  >0 0<  (__.'\"\"\"\"\"\"\n  \" ` \" \"_",
        "     .---.\n  (\\./)     \\.......-\n  >^ ^<  (__.'\"\"\"\"\"\"\n  \" ` \" \"_",
    };

  char *messages[] = {
    "squeak!",
    "squeak! !",
    "squeak",
    "squeak...",
    "SQUEAK",
    "SQUEAK!!",
    "SQUEAAAKKKKK",
    };

    char *funfact[] = {
        "press q to exit games !",
        "rats love cheese!",
        "i am sleepy.."
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
        rat.hunger = 80.0f;
        rat.love   = 80.0f;
        rat.fun    = 80.0f;
        rat.clean  = 80.0f;
        rat.health = 80.0f;
        rat.age    = time(NULL);
        return;
    }
    if(fscanf(f, "%f\n%f\n%f\n%f\n%f\n%d\n", 
           &rat.hunger, &rat.love, &rat.fun, &rat.clean, &rat.health, &rat.age) != 6){
    fprintf(stderr, "error: failed to initialize save data.\n");
    }    
    //this should shut the compiler up 

    fclose(f);
}

float decay(float current, float dayspassed, float rate) {
    float result = current - (dayspassed * rate);
    return result < 0.0f ? 0.0f : result;
}

void setstat(){
    int timenow = time(NULL);
    int newage = timenow - rat.age;
    float days = newage / 86400.0f; //one day 

    rat.hunger = decay(rat.hunger, days, 50.0f); //hungry beast
    rat.love   = decay(rat.love,   days, 1.1f); //rats love you they won't hate you easly 
    rat.fun    = decay(rat.fun,    days, 25.0f);  
    rat.clean  = decay(rat.clean,  days, 5.0f);//rats are surprisingly clean they clean themselves so slow  
    rat.health = decay(rat.health, days, 0.6f);  

    rat.age = timenow;
}  
//this is better yeah

void printrat(WINDOW *win){
 
    werase(win);
    box(win, 0, 0);

    int ratties = (sizeof(ratart)/sizeof(ratart[0])); 
    int row = 1;
    char artcopy[256];
    strncpy(artcopy, ratart[rand() % ratties], sizeof(artcopy));
    artcopy[sizeof(artcopy)-1] = '\0';
 
    char *line = strtok(artcopy, "\n");
    while(line){
        mvwprintw(win, row++, 2, "%s", line);
        line = strtok(NULL, "\n");
    }
 
    row++;
    int howmany = sizeof(messages) / sizeof(messages[0]);
    int facts = sizeof(funfact) / sizeof(funfact[0]);
    mvwprintw(win, row++, 2, "%s", messages[rand() % howmany]);
 
    row++;
    mvwprintw(win, row++, 2, "hunger: %.2f", rat.hunger);
    mvwprintw(win, row++, 2, "love:   %.2f", rat.love);
    mvwprintw(win, row++, 2, "fun:    %.2f", rat.fun);
    mvwprintw(win, row++, 2, "clean:  %.2f", rat.clean);
    mvwprintw(win, row++, 2, "health: %.2f", rat.health);
 
    row++;
    mvwprintw(win, row++, 2, "1: play guess the number! (fun up)");
    mvwprintw(win, row++, 2, "2: go to the maze (hunger up)");
    mvwprintw(win, row++, 2, "4: take a shower! (clean up)");
    mvwprintw(win, row++, 2, "5: clicker the cheese! (all up)");
    mvwprintw(win, row++, 2, "9: print me! (refreshes stats)");
    mvwprintw(win, row++, 2, "0: exit...");
    mvwprintw(win, row++ +1, 1, "fun fact: %s", funfact[rand() % facts]);
 
    wrefresh(win);
}
 
 
int main(){
srand(time(NULL));
int truing = 1;
 
    load();
    setstat();
 
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
 
    WINDOW *win = newwin(25, 45, 1, 1);
    printrat(win);
 
    while(truing){
        int choice = wgetch(win);
 
        switch(choice){
 
        case '1':
            endwin();
            gameguess();
            refresh();
            break;
        case '2':
            endwin();
            gamemaze();
            //printf("the maze is really dark squeak...\n\n");
            refresh();
            break;
        case '4':
            endwin();
            gamecatch();
            //printf("your rat feels cleaner!\n\n");
            refresh();
            break;
        case '5':
            endwin();
            gameclicker();
            refresh();
            break;
        case '9': break;
        case '0': truing = 0; break;
        }
// i will put those printfs where the squeaks go but i have no clue how to do that today  
        if(truing) printrat(win);
    }
 
    delwin(win);
    endwin();
 
    save();
    printf("bye bye~~\n");
    return(0);
}

