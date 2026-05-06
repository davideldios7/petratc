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
    int W = (width / 3);
    int H = (height / 2);
 
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            grid[y][x] = '#';
 
    int cw = (W - 1) / 3; //make them 2x2 instead of 1x1
    int ch = (H - 1) / 3;
    if (cw < 1 || ch < 1) return;
 
    int total = cw * ch;
    char *vis = calloc(total, 1); //visited
    int  *px  = malloc(total * sizeof(int)); //path x
    int  *py  = malloc(total * sizeof(int)); //path y
    int  *ip  = malloc(total * sizeof(int)); // index of path 
    vis[0] = 1;
    grid[1][1] = grid[1][2] = grid[2][1] = grid[2][2] = ' '; //these should be open always 
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

        //each cell as 2x2
        for (int i = 0; i < pl; i++) {
            int gx = 3*px[i]+1, gy = 3*py[i]+1;
            grid[gy  ][gx] = grid[gy  ][gx+1] = ' ';
            grid[gy+1][gx] = grid[gy+1][gx+1] = ' ';
            vis[py[i]*cw+px[i]] = 1;
        }
        vc += pl;

        for (int i = 0; i + 1 < pl; i++) {
            int ax=px[i], ay=py[i], bx=px[i+1], by=py[i+1];
            if (bx != ax) {
                int wc = 3*(ax+bx+1)/2;
                grid[3*ay+1][wc] = grid[3*ay+2][wc] = ' ';
            } else {
                int wr = 3*(ay+by+1)/2;
                grid[wr][3*ax+1] = grid[wr][3*ax+2] = ' ';
            }
        }//carve 2 wide between consecutive path cells

        //connect last path cell to the already visited cell
        if (pl > 0) {
            int ax=px[pl-1], ay=py[pl-1];
            if (cx != ax) {
                int wc = 3*(ax+cx+1)/2;
                grid[3*ay+1][wc] = grid[3*ay+2][wc] = ' ';
            } else {
                int wr = 3*(ay+cy+1)/2;
                grid[wr][3*ax+1] = grid[wr][3*ax+2] = ' ';
            }
        }
    }
 
    //entrance top, exit bottom
    if (H > 0) { grid[0][1] = grid[0][2] = ' '; }
    if (H > 3 && W > 3) { grid[H-1][W-3] = grid[H-1][W-2] = ' '; } 
    free(vis); free(px); free(py); free(ip);
}

//i have no fucking idea what i did here. it was revealed to me in a dream unironically 
//i have a slight idea of what i have done here

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
