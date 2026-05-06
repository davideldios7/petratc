#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
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

static char **grid; //big ahh grid

void lerw() {
    int W = getmaxx(stdscr);
    int H = getmaxy(stdscr);
 
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            grid[y][x] = '#';
 
    int cw = W / 2;
    int ch = H / 2;
    if (cw < 1 || ch < 1) return;
 
    int total = cw * ch;
    char *vis = calloc(total, 1);
    int  *px  = malloc(total * sizeof(int));
    int  *py  = malloc(total * sizeof(int));
    int  *ip  = malloc(total * sizeof(int));  
    vis[0] = 1;
    grid[1][1] = ' ';
    int vc = 1;
 
    int ddx[] = { 1, -1,  0,  0 };
    int ddy[] = { 0,  0,  1, -1 };
 
    while (vc < total) {
        int sx, sy;
        do {
            sx = rand() % cw;
            sy = rand() % ch;
        } while (vis[sy * cw + sx]);
 
        memset(ip, -1, total * sizeof(int));
        px[0] = sx; py[0] = sy;
        ip[sy * cw + sx] = 0;
        int pl = 1;
        int cx = sx, cy = sy;
 
        while (!vis[cy * cw + cx]) {
            int dir = rand() % 4;
            int nx = cx + ddx[dir];
            int ny = cy + ddy[dir];
            if (nx < 0 || nx >= cw || ny < 0 || ny >= ch) continue;
 
            int ni = ny * cw + nx;
            if (ip[ni] >= 0) {
                //loop: erase back to ip[ni]  heh ni hehjehehe
                int idx = ip[ni];
                for (int i = idx + 1; i < pl; i++)
                    ip[py[i] * cw + px[i]] = -1;
                pl = idx + 1;
                cx = nx; cy = ny;
            } else {
                px[pl] = nx; py[pl] = ny;
                ip[ni] = pl++;
                cx = nx; cy = ny;
            }
        }
 
        for (int i = 0; i < pl; i++) {
            grid[py[i]*2+1][px[i]*2+1] = ' ';
            vis[py[i]*cw+px[i]] = 1;
        }
        vc += pl;
 
        for (int i = 0; i + 1 < pl; i++)
            grid[py[i]+py[i+1]+1][px[i]+px[i+1]+1] = ' ';
 
        if (pl > 0)
            grid[py[pl-1]+cy+1][px[pl-1]+cx+1] = ' ';
    }
 
    //entrance top, exit bottom
    if (H > 0) grid[0][1] = ' ';
    if (H > 1 && W > 2) grid[H-1][W-2] = ' ';
 
    free(vis); free(px); free(py); free(ip);
}

//i have no fucking idea what i did here. it was revealed to me in a dream unironically 

void gamemaze(){ 

    srand(time(NULL));
        static int initialized = 0;
        if(initialized == 0){ initscr();}else{refresh();}
        ++initialized;

    int W = getmaxx(stdscr);
    int H = getmaxy(stdscr);

    grid = malloc(H * sizeof(char *)); //those who allocate memory 
    for (int y = 0; y < H; y++)
    grid[y] = malloc(W * sizeof(char));

    lerw();

    found = 0; 

    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    cursor curs = {1, 1, {"@"}};

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


        for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
        if (grid[y][x] == '#') mvaddch(y, x, '#');

        mvaddstr(curs.posy, curs.posx, curs.face[0]);

        refresh();
        usleep(50000);
    }

    for (int y = 0; y < H; y++)
    free(grid[y]);
    free(grid);

stop(); 
}
