#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include "../rat.h"

#define width getmaxx(stdscr)
#define height getmaxy(stdscr)

int running;
int row = 1; 

static void stop() {
    clear();
    refresh();
    endwin();
    running = 0; 
}

void gameclicker(){

    srand(time(NULL));
        static int initialized = 0;
        if(!initialized){ initscr();}else{refresh();}
        ++initialized;

    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    int winh = height * 0.96;
    int winw = width * 0.96;
    int boxy = (height - winh) / 2;
    int boxx = (width - winw) / 2;
    WINDOW *win = newwin(winh, winw, boxy, boxx);
   
    box(win, 0, 0); //box? 

    //should i wrap all this into a macro in rat.h to not have to write all of this every time i make a new game?
    
    running = 1; 

    mvwprintw(win, row ++, 2, "hi"); 
    while(running){
        if(!running){stop();}
        int ch = getch();     
        switch (ch) {
            case 'q': case 'Q': stop(); break;
            }

    
        wrefresh(win);
        usleep(16667); //60 fps according to google ai overview lmao
    }

stop();
}
