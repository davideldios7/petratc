#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <sys/stat.h>
#include "rat.h"


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


Rat rat;
/*i was cleaning the code up a bit and what the fuck is this
//it's so funny Rat rat 
*/
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

    WINDOW *win = ratdrawbox();
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
            /*printf("the maze is really dark squeak...\n\n");*/
            refresh();
            break;
        case '4':
            endwin();
            gamecatch();
            /*printf("your rat feels cleaner!\n\n");*/
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
/* i will put those printfs where the squeaks go but i have no clue how to do that today  */
        if(truing) printrat(win);
    }
 
    delwin(win);
    endwin();
 
    save();
    printf("bye bye~~\n");
    return(0);
}
