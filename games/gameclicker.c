#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include "../rat.h"

/*
i lowkey stole the art of the little rat and the cheese from the internet lol
https://asciiart.website/art/752
*/

int running;

static void stop() {
    clear();
    refresh();
    endwin();
    running = 0; 
}

static char *cheeseart = {
" ()()         ____\n (..)        /|o  |\n /\\/\\       /o|  o|\nc\\db/o...  /o_|_o_|"
};
//i hate this 

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

    while (fabs(val) >= 1000) {
        val /= 1000.0;
        y++;
    }
    while (val != 0.0 && fabs(val) < 1) {
        val *= 1000.0;
        y--;
    }

    b.x = val;
    b.y = y;
    return b;
}

static bignum normalizebignum(bignum b){
    double val = b.x;
    int y = b.y;
 
    while (fabs(val) >= 1000) {
        val /= 1000.0;
        y++;
    }
    while (val != 0.0 && fabs(val) < 1) {
        val *= 1000.0;
        y--;
    }
 
    b.x = val;
    b.y = y;
    return b;
}
 
static bignum addbignum(bignum a, bignum b){
    bignum result;
    if(a.y == b.y){
        result.x = a.x + b.x;
        result.y = a.y;
    } else if(a.y > b.y){
        double bx = b.x;
        for(int i = a.y - b.y; i > 0; i--) bx /= 1000.0;
        result.x = a.x + bx;
        result.y = a.y;
    } else {
        double ax = a.x;
        for(int i = b.y - a.y; i > 0; i--) ax /= 1000.0;
        result.x = ax + b.x;
        result.y = b.y;
    }
    return normalizebignum(result);
}
 
static bignum subbignum(bignum a, bignum b){
    bignum result;
    if(a.y == b.y){
        result.x = a.x - b.x;
        result.y = a.y;
    } else if(a.y > b.y){
        double bx = b.x;
        for(int i = a.y - b.y; i > 0; i--) bx /= 1000.0;
        result.x = a.x - bx;
        result.y = a.y;
    } else {
        double ax = a.x;
        for(int i = b.y - a.y; i > 0; i--) ax /= 1000.0;
        result.x = ax - b.x;
        result.y = b.y;
    }
    return normalizebignum(result);
}
 
static bignum mulbignum(bignum a, bignum b){
    bignum result;
    result.x = a.x * b.x;
    result.y = a.y + b.y;
    return normalizebignum(result);
}
 
static bignum divbignum(bignum a, bignum b){
    bignum result;
    result.x = a.x / b.x;
    result.y = a.y - b.y;
    return normalizebignum(result);
}
 
static bignum addmixregbig(bignum a, double regnum){
    return addbignum(a, normalize(regnum));
}
 
static bignum submixregbig(bignum a, double regnum){
    return subbignum(a, normalize(regnum));
}
 
static bignum mulmixregbig(bignum a, double regnum){
    return mulbignum(a, normalize(regnum));
}
 
static bignum divmixregbig(bignum a, double regnum){
    return divbignum(a, normalize(regnum));
}

static char *bignumprint(bignum b) {
    //32 bytes is more than enough for 124.24 trillion idk
    char *nice = malloc(32 * sizeof(char)); 
    if(nice == NULL) return NULL;

    if(b.y >= 0 && b.y < 12) {
        snprintf(nice, 32, "%.2lf%s", b.x, digit[b.y]);
    } else{
        snprintf(nice, 32, "%.2lfe3*%d", b.x, b.y);
    }
    return nice; 
}


//wide = art and score side by side
//narrow = art on topand score below
typedef enum { layoutnarrow, layoutwide } layoutmode;

//bounding box of the drawn cheese art in winlocal coordinates for click detection
static int artrow0, artcol0, artrows, artcols;

static layoutmode getlayout(WINDOW *win){
    return (getmaxx(win) >= getmaxy(win) * 2) ? layoutwide : layoutnarrow;
}

static void drawwin(WINDOW *win, bignum cheeses){

    werase(win);
    box(win, 0, 0);

    int winw = getmaxx(win);
    layoutmode mode = getlayout(win);

    char artcopy[2048];
    strncpy(artcopy, cheeseart, sizeof(artcopy));
    artcopy[sizeof(artcopy)-1] = '\0';

    int artcol = 2;
    int artrow = 1;
    int scorecol, scorewrapw;

    if(mode == layoutwide){
        scorecol = winw / 2 + 2;
        scorewrapw = winw - scorecol - 2;
    } else {
        scorecol = 2;
        scorewrapw = winw - 4;
    }

    int row = artrow;
    int widest = 0;
    char *line = strtok(artcopy, "\n");
    while(line){
        mvwprintw(win, row, artcol, "%s", line);
        int len = strlen(line);
        if(len > widest) widest = len;
        row++;
        line = strtok(NULL, "\n");
    }

    //save where the art landed so the click handler knows what to hit-test against
    artrow0 = artrow;
    artcol0 = artcol;
    artrows = row - artrow;
    artcols = widest;

    char *scorestr = bignumprint(cheeses);
    char scoremsg[64];
    snprintf(scoremsg, sizeof(scoremsg), "cheese: %s", scorestr);
    free(scorestr);

    if(mode == layoutwide){
        mvwprintw(win, 1, scorecol, "%s", scoremsg);
        wrapprint(win, 3, scorecol, scorewrapw, "click the cheese!");
    } else {
        int scorerow = artrow0 + artrows + 1;
        wrapprint(win, scorerow, scorecol, scorewrapw, scoremsg);
        wrapprint(win, scorerow+1, scorecol, scorewrapw, "click the cheese!");
    }

    wrefresh(win);
}

void gameclicker(){

    srand(time(NULL));

    WINDOW *win = ratdrawbox();
    mousemask(BUTTON1_PRESSED, NULL);
    mouseinterval(0); //don't wait to resolve single vs double click

    running = 1;

    bignum cheeses = {0.0, 0};
    bignum onecheese = {1.0, 0}; //what a single click is worth

    drawwin(win, cheeses);

    while(running){
        if(!running){stop(); break;}

        int ch;
        while((ch = getch()) != ERR){ //drain the whole queue each frame so rapid clicks/keys aren't ignroed 
            switch (ch) {
                case 'q': case 'Q': stop(); break;
                case 'a': cheeses = addmixregbig(cheeses, 50.0); break;
                case 'b': cheeses = mulmixregbig(cheeses, 235.2); break;
                case KEY_MOUSE: {
                    MEVENT event;
                    if(getmouse(&event) == OK && (event.bstate & BUTTON1_PRESSED)){
                        int y = event.y, x = event.x;
                        //translate the screen click into winlocal coordinates
                        if(wmouse_trafo(win, &y, &x, FALSE)){
                            if (y >= artrow0 && y < artrow0 + artrows &&
                               x >= artcol0 && x < artcol0 + artcols){
                                cheeses = addbignum(cheeses, onecheese);
                            }
                        }
                    }
                    break;
                }
            }
            if(!running) break;
        }

        if(running) drawwin(win, cheeses);

        usleep(16667); //60 fps according to google ai overview lmao
    }

stop();
}
