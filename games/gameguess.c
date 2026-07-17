#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include "../rat.h"

#define boxw 45
#define boxh 25
#define maxhistory 4

static int wrapprint(WINDOW *win, int row, int col, int maxwidth, const char *msg){

    char copy[256];
    strncpy(copy, msg, sizeof(copy));
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

        if(linelen > 0) strcat(line, " ");
        strcat(line, word);

        word = strtok(NULL, " ");
    }
    if(strlen(line) > 0){
        mvwprintw(win, row++, col, "%s", line);
    }

    return row;
}

static void drawwin(WINDOW *win, int artpick, const char *msg, int history[], int histcount){

    werase(win);
    box(win, 0, 0);

    int row = 1;
    char artcopy[256];
    strncpy(artcopy, ratart[artpick], sizeof(artcopy));
    artcopy[sizeof(artcopy)-1] = '\0';

    char *line = strtok(artcopy, "\n");
    while(line){
        mvwprintw(win, row++, 2, "%s", line);
        line = strtok(NULL, "\n");
    }

    row++;
    wrapprint(win, row, 2, boxw - 4, msg);

    int inputrow = boxh - 3;


    for(int i = 0; i < histcount; i++){
        mvwprintw(win, inputrow - histcount + i, 2, "%d", history[i]);
    }

    wrefresh(win);
}

void gameguess(){

    int guess;
    int solved = 0;
    int range = 100;

    int correct = (rand() % (range + 1));

    int history[maxhistory];
    int histcount = 0;

    static int initialized = 0;
    if(initialized == 0){ initscr(); }else{ refresh(); }
    ++initialized;

    noecho();
    cbreak();
    curs_set(0);

    WINDOW *win = newwin(boxh, boxw, 1, 1);

    char msg[96];
    snprintf(msg, sizeof(msg), "let's play a game, squeak!~ you gues a number between 0 and %d!", range);
    drawwin(win, 4, msg, history, histcount);

    int quitgame = 0;

    while(solved != 1){

        echo();
        char buf[16];
        mvwgetnstr(win, boxh - 3, 2, buf, sizeof(buf) - 1);
        noecho();

        if((buf[0] == 'q' || buf[0] == 'Q') && buf[1] == '\0'){
            drawwin(win, 2, "are you suuure you don't wanna play anymoreee~?", history, histcount);

            echo();
            char confirm[16];
            mvwgetnstr(win, boxh - 3, 2, confirm, sizeof(confirm) - 1);
            noecho();

            if((confirm[0] == 'q' || confirm[0] == 'Q' || confirm[0] == 'y' || confirm[0] == 'Y') && confirm[1] == '\0'){
                quitgame = 1;
                break;
            } else if((confirm[0] == 'n' || confirm[0] == 'N') && confirm[1] == '\0'){
                drawwin(win, 4, msg, history, histcount);
                continue;
            } else if(sscanf(confirm, "%d", &guess) != 1){
                drawwin(win, 4, "that's not a number, try again", history, histcount);
                continue;
            }
        } else if(sscanf(buf, "%d", &guess) != 1){
            drawwin(win, 4, "that's not a number, try again", history, histcount);
            continue;
        }
        //this is too "complicated" for no reason at all

        int diff = abs(guess - correct);
        int artpick;
        if(diff <= 10){
            artpick = 3;
        } else if(diff > 30){
            artpick = 4;
        } else {
            artpick = rand() % 2;
        }

        if(guess < correct){
            if(histcount < maxhistory){
                history[histcount++] = guess;
            } else {
                for(int i = 0; i < maxhistory - 1; i++) history[i] = history[i+1];
                history[maxhistory-1] = guess;
            }
            drawwin(win, artpick, "number too low try again", history, histcount);
        }else if(guess > correct){
            if(histcount < maxhistory){
                history[histcount++] = guess;
            } else {
                for(int i = 0; i < maxhistory - 1; i++) history[i] = history[i+1];
                history[maxhistory-1] = guess;
            }
            drawwin(win, artpick, "number too high try again", history, histcount);
        }else {
            drawwin(win, artpick, "yes!! correct! squeaaakk!~", history, histcount);
            solved = 1;
        }
    }

    if(!quitgame){
        usleep(2000000);
        addstat(&rat.fun);
    }

    delwin(win);
    endwin();

}
