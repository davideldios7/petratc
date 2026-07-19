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

/*
    //i need to make a system based on intergers to represent non-int numbers like 0.5 or  five trillion as 5T instead of 500000000000 or whatever  
    //how do i tell a computer that 1000 = 1k and 10000 = 10k and 1000000 = 1M and that etc
    //i'll use powers of 10 
    //1^10x0 = 1 1^1(10x1) = 10 1^1(10x2) = 100 1^1(10x3) = 1k 
    //  x*10^y <-- this is what i store 
    //1 becomes X. x can range from 0 to 999, when it reaches 999 it adds 1 to y and x goes back to 0 
    //is this memory safe????? yeah sure i don't see why it wouldnt
    //x is a float or a double i think a double yeah a double
    //
    //and then print that as it should so 2.3^10x3 = 2.3k 
    //and also other functions to add or substract from this using regular numbers
    
    //wait i could just do powers of 1000 
    //x is still 0.0 - 999.999 but it's x*1000^y 
    //55.3* 1000^2 = 55300000
    //
    //once x reaches 999.999 it wraps back to 0 and adds 1 to y 
    //then check y so that y = 0 is units, y = 1 is k, y = 2 is M, y = 3 is B, y = 4 is T, etc
*/

/*
//a bignum is just a scientific notation number
// x ^ 1000^y 
// 55300 = 55.3*1000^1
// double x = 55.3 
// int y = 1 
*/
typedef struct bignum{
    double x;
    int y;
}bignum; 

static char *digit[] = {
    "", "K", "M", "B", "T",
    "Qa", "Qi", "Sx", "Sp", 
    "Oc", "No", "Dc"  
    // up to y=11 (10^36) there's no way someone gets farther than this
};

static bignum normalize(double regnum){
    bignum b;
    double val = regnum;
    int y = 0;

    while (abs((int)val) >= 1000) {
        val /= 1000.0;
        y++;
    }
    while (val != 0.0 && abs((int)val) < 1) {
        val *= 1000.0;
        y--;
    }

    b.x = val;
    b.y = y;
    return b;
}

static double weirderize(bignum a){
    double result = a.x;
    int y = a.y;
    while (y > 0) { result *= 1000.0; y--; }
    while (y < 0) { result /= 1000.0; y++; }
    return result;
}

static bignum addbignum(bignum a, bignum b){
    return normalize(weirderize(a) + weirderize(b));
}

static bignum subbignum(bignum a, bignum b){
    return normalize(weirderize(a) - weirderize(b));
}

static bignum mulbignum(bignum a, bignum b){
    return normalize(weirderize(a) * weirderize(b));
}

static bignum divbignum(bignum a, bignum b){
    return normalize(weirderize(a) / weirderize(b));
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

    int winh = height * 0.97;
    int winw = width * 0.97;
    int boxy = (height - winh) / 2;
    int boxx = (width - winw) / 2;
    WINDOW *win = newwin(winh, winw, boxy, boxx);
   
    box(win, 0, 0); //box? 

    //should i wrap all this into a macro in rat.h to not have to write all of this every time i make a new game?
    //or just a function in something like rat.c yeah 
    
    running = 1;

    bignum cheeses;

    mvwprintw(win, row ++, 4, "hi"); 
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
