#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include "../rat.h"

    /*
    // nico told me to add more comments on my code i hope he'll be happy with this
    //
    // a 2d array holds the position of all objects on screen
    // many raindrops have a small chance of spawning every tick
    // they should have a slightly random accelaration for each new instance
    // the player can only move side to side if everything goes according to plan
    //
    // the screen shouldn't refresh each time but it instead updates the relevant spots like the player and raindrop 
    //otherwise it would flicker (this happen's on nico's game)
    //
    //or actually it's okay to just flush the whole screen i think
    //
    //i'll just use termios grok told me it would be like 20x easier than coding everyhing
    //
    // fixed screen size? what if whoever plays this plays it on a tiny tiny screen... maybe set a max value and then somehow 
    //figure out the terminal size to display whatever ammount of spaces properly (i doubt this so i'll make the screen tiny from the start)
    //
    // heh the player cursor will be (>.<)_ for the rat its hitbox will be 3 rows wide (the center is the little dot) 
    //
    // this program should be coded like if it was the 90s. 
    //if i whip out an i486 machine i should be able to compile it assuming it runs linux at least 4 
    //
    // what if the display loop is just print the array of the screen each step and somehow have the cursor separate from that array (or have it in the array too) 
    //then it flushes to sdout like that one other program i made does that'd be easier methinks
    //
    // i just realized this is basically cmatrix with a little guy at the bottom
    */

#define maxdrops 70

static int running;
static int won;
static int howmany; 
static int width;
static int height; 

typedef struct{

    int posx;
    int posy;
    int active;
    char *symbol; 

}drop;

typedef struct{
    int posx;
    char *face[2];
    int direction;
}cursor; 

static drop droop[maxdrops]; 
void spawndrops() {
    for (int i = 0; i < maxdrops; i++) {
        if (!droop[i].active) {
            droop[i].posx = 1 + rand() % (width - 2);
            droop[i].posy = 0;
            droop[i].active = 1;
            char *symbols[] = {"O", "0", "o"};
            droop[i].symbol = symbols[rand() % 3];
            break;
        }
    }
}

static void stop() {
    clear();
    refresh();
    endwin();
    if(won){addstat(&rat.clean); printf("the rat feels cleaner...\n\n");}
    won = 0;
    running = 0;
}



void gamecatch(){
   
    srand(time(NULL)); //@sagucs hey 

    WINDOW *win = ratdrawbox();
    howmany = 0; 
    width = 40;
    height = getmaxy(win) - 2;

    cursor curs = {width / 2, {"(>.<)_", "_(>.<)"}, 0};

    for (int i = 0; i < maxdrops; i++) droop[i].active = 0;

    running = 1;
    while (running) {
        if(!running){stop();} 
        int ch = getch();
        switch (ch) {
            case 'q': case 'Q': stop(); break;
            case 'a': case 'A': case KEY_LEFT:
                curs.posx-= 2;
                if(curs.posx < 0) curs.posx = width - 6;
                curs.direction = 0;
                break;
            case 'd': case 'D': case KEY_RIGHT:
                curs.posx += 2;
                if(curs.posx > width - 6) curs.posx = 0;
                curs.direction = 1 ; 
                break;
        }


            if (!running) break;
        if (rand() % 10 < 3) spawndrops();

        werase(win);
        box(win, 0, 0);

        mvwvline(win, 1, width + 1, ACS_VLINE, height);
        mvwaddch(win, 0, width + 1, ACS_TTEE);
        mvwaddch(win, height + 1, width + 1, ACS_BTEE);
        //this should do it

        mvwprintw(win, 1, width + 3, "caught: %d/30", howmany);
        mvwprintw(win, 2, width + 3, "(catch 30 to win!)");
        mvwprintw(win, 3, width + 3, "move with A and D");
        mvwprintw(win, 4, width + 3, "or the or the corresponding arrows");



        for (int i = 0; i < maxdrops; i++) {
            if (droop[i].active) {
                mvwaddstr(win, droop[i].posy + 1, droop[i].posx + 1, droop[i].symbol);
                droop[i].posy++;
                if (droop[i].posy >= height) droop[i].active = 0;
                
                if (droop[i].posy == height - 1 && 
                    droop[i].posx >= curs.posx && 
                    droop[i].posx <= curs.posx + 5) {
                    droop[i].active = 0;
                    howmany++;
                    if(howmany == 30){won = 1; running = 0; stop();}
                }
            }
        }

        mvwaddstr(win, height, curs.posx + 1, curs.face[curs.direction]);

    wrefresh(win); 
    usleep(100000);    
    }

stop(); 
}
