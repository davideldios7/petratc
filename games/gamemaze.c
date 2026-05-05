#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include "../rat.h"

/*
// i'm probably gonna use a loop erased random walk algorithm
//the screen will start as 100% walls then slowly build a path 
//(i have no idea how many resources this will take, if it's slow i'll ask grok to multithread it)
//
// a couple ammount of cheeses will spawn in places where there are no walls 
// eating cheeses will add to the base addstat() but addstat() only will be called if 1 cheese is eaten
// idk what else to do here. 
// you start at the top and end at the bottom if i ever wanna automate taking care of the rat
*/

#define width getmaxx(stdscr)
#define height getmaxy(stdscr)

static int running;
static int won;
static int found; 

typedef struct{

    int posy;
    int posx;
    char *face[1];

}cursor;

static void stop() {
    clear();
    refresh();
    endwin();
    if(won){addstat(&rat.hunger); addstat(&rat.fun); printf("the rat feels less hungry...\n\n");}
    won = 0;
    found = 0;
    running = 0; 
}
//yes i stole the code from gamecatch shut up it works 





void gamemaze(){ 

    srand(time(NULL));
        static int initialized = 0;
        if(initialized == 0){ initscr();}else{refresh();}
        ++initialized;

    found = 0; 

    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    cursor curs = {0, 0, {"@"}};

    running = 1;
    
    while (running) {
        if (!running) stop();
        int ch = getch();
        switch (ch) {
            case 'q': case 'Q': stop(); break;
            case 'a': case 'A': case KEY_LEFT:
                curs.posx -= 1; break;
            case 'd': case 'D': case KEY_RIGHT:
                curs.posx += 1; break;
            case 'w': case 'W': case KEY_UP:
                curs.posy -= 1; break; 
            case 's': case 'S': case KEY_DOWN:
                curs.posy += 1; break; 
        }
        
        if (!running) break;
        clear();

        mvaddstr(curs.posy, curs.posx, curs.face[0]);

        refresh();
        usleep(50000);
    }

stop(); 
}
