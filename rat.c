#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <sys/stat.h>
#include "rat.h"


/*functions used in initilization/main menu (not games)*/
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
    /*this should shut the compiler up*/

    fclose(f);
}

void addstat(float *guy){

    if(*guy < 98.5){
        *guy += 1.5;
    } else{*guy += 0.05;}     
}

float decay(float current, float dayspassed, float rate) {
    float result = current - (dayspassed * rate);
    return result < 0.0f ? 0.0f : result;
}

void setstat(){
    int timenow = time(NULL);
    int newage = timenow - rat.age;
    float days = newage / 86400.0f; /*one day*/

    rat.hunger = decay(rat.hunger, days, 50.0f); /*hungry beast*/
    rat.love   = decay(rat.love,   days, 1.1f); /*rats love you they won't hate you easly*/
    rat.fun    = decay(rat.fun,    days, 25.0f);  
    rat.clean  = decay(rat.clean,  days, 5.0f);/*rats are surprisingly clean they clean themselves so slow*/
    rat.health = decay(rat.health, days, 0.6f);  

    rat.age = timenow;
}

WINDOW *ratdrawbox(void){

    static int initialized = 0;
    if(!initialized){ initscr(); }else{ refresh(); }
    ++initialized;

    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    int winh = getmaxy(stdscr) * 0.97;
    int winw = getmaxx(stdscr) * 0.97;
    int boxy = (getmaxy(stdscr) - winh) / 2;
    int boxx = (getmaxx(stdscr) - winw) / 2;

    return newwin(winh, winw, boxy, boxx);
}

int wrapprint(WINDOW *win, int row, int col, int maxwidth, const char *msg){
    if(msg == NULL) return row;

    char copy[256];
    strncpy(copy, msg, sizeof(copy) - 1);
    copy[sizeof(copy)-1] = '\0';

    char line[256] = "";
    char *word = strtok(copy, " ");
    while(word){
        int linelen = strlen(line);
        int wordlen = strlen(word);
        int extra = (linelen > 0) ? 1 : 0;
        if(linelen + extra + wordlen > maxwidth){
            mvwprintw(win, row++, col, "%s", line);
            line[0] = '\0';
            linelen = 0;
            extra = 0;
        }

        if(linelen > 0) {
            if(strlen(line) + 1 < sizeof(line)) {
                strcat(line, " ");
            }
        }
        if(strlen(line) + strlen(word) < sizeof(line)) {
            strcat(line, word);
        }

        word = strtok(NULL, " ");
    }
    if(strlen(line) > 0){
        mvwprintw(win, row++, col, "%s", line);
    }

    return row;
}
