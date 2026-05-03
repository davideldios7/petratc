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

#define width 40;
#define height 50; 

int running;

typedef struct{

    int posx;
    int posy;
    char *symbol[3]; 

}drop;

typedef struct{
    int posx;
    char *face[2];
}cursor; 

void spawndrops(){

return; 

}

void stop() {
    if (!isendwin()) {
        endwin();
    }
    addstat(&rat.clean);
    running = 0;
}



void gamecatch(){
   
    srand(time(NULL)); //@sagucs hey 
        
    static int initialized = 0;
        if(initialized == 0){ initscr();}else{refresh();} // turns out you don't wanna call initscr() many times per session so do this because it glitches otherwise 
    ++initialized;

    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    drop droop = {0, 0, {"O", "0", "o"}}; //on spawn random which symbol it'll use 
    cursor curs = {20, {"(>.<)_", "_(>.<)" }}; //face[0] is going left face[1] is going right 


    running = 1;
    while (running) {
        int ch = getch();
        switch (ch) {
            case 'q': case 'Q': stop(); break;
            case 'a': case 'A': case KEY_LEFT:
                curs.posx--;
                break;
            case 'd': case 'D': case KEY_RIGHT:
                curs.posx++;
                break;
        }
    }

stop(); 
}
