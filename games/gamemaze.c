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

static int running;
static int won;
static int found; 
static int width;
static int height;
static WINDOW *win; 

typedef struct{

    int posy;
    int posx;
    char *face[1];

}cursor;


static void stop() {
    clear();
    refresh();
    endwin();
    if(won){addstat(&rat.fun);}
    won = 0;
    found = 0;
    running = 0; 
}
//yes i stole the code from gamecatch shut up it works 

static char **grid; //big ahh grid

void lerw() {
    int W = (width / 3);
    int H = (height / 2);

    int cw = (W - 1) / 3; //make them 2x2 instead of 1x1
    int ch = (H - 1) / 2;
    if (cw < 1 || ch < 1) return;

    W = 3 * cw + 1; //trim w to exactly fit
    H = 3 * ch + 1; 
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            grid[y][x] = '#';;
 
    int total = cw * ch;
    char *vis = calloc(total, 1); //visited
    int  *px  = malloc(total * sizeof(int)); //path x
    int  *py  = malloc(total * sizeof(int)); //path y
    int  *ip  = malloc(total * sizeof(int)); // index of path

    if(vis == NULL || px == NULL || py == NULL || ip == NULL) {
        free(vis);
        free(px);
        free(py);
        free(ip);
        return;
    }
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

static int iswall(int y, int x) {
    return grid[y][x] == '#';
}

static int ischeese(int y, int x){
    return grid[y][x] == 'O';
}

void spawncheeses() {
    int W = (width / 3);
    int H = (height / 2);
    int cw = (W - 1) / 3;
    int ch = (H - 1) / 2;
    W = 3 * cw + 1;
    H = 3 * ch + 1;

    int placed = 0;
    while (placed < 5) {
        int x = rand() % W;
        int y = rand() % H;
        if (grid[y][x] == ' ') {
            grid[y][x] = 'O';
            placed++;
        }
    }
}

void eatcheese(cursor *curs) {
    if (ischeese(curs->posy, curs->posx)) {
        grid[curs->posy][curs->posx] = ' ';
        found++;
        addstat(&rat.hunger);
    }
}

void checkexit(cursor *curs) {
    int w = ((width/3) - 1) / 3;
    int h = ((height/2) - 1) / 2;
    int W = 3 * w + 1;
    int H = 3 * h + 1;
    if (curs->posy == H - 1 && (curs->posx == W - 3 || curs->posx == W - 2)) {
        won = 1;
        stop();
    }
}

static void properrestart(cursor *curs, int *W, int *H) {
    for (int y = 0; y < *H; y++) free(grid[y]);
    free(grid);

    width = getmaxx(win) - 2;
    height = getmaxy(win) - 2;
    *W = width;
    *H = height;

    grid = malloc(*H * sizeof(char *));
    if(grid == NULL) {
        running = 0; 
        return;
    }
    for (int y = 0; y < *H; y++) {
        grid[y] = malloc(*W * sizeof(char));
        if(grid[y] == NULL) {
            // Free already allocated rows
            for(int i = 0; i < y; i++) free(grid[i]);
            free(grid);
            running = 0; 
            return;
        }
    }

    lerw();
    spawncheeses();

    curs->posy = 1;
    curs->posx = 1;
    found = 0;
}

void gamemaze(){ 

    srand(time(NULL));

    win = ratdrawbox();
    width = getmaxx(win) - 2;
    height = getmaxy(win) - 2;

    int W = width;
    int H = height;

    grid = malloc(H * sizeof(char *)); //those who allocate memory
    if(grid == NULL) {
        stop();
        return;
    }
    for (int y = 0; y < H; y++) {
        grid[y] = malloc(W * sizeof(char));
        if(grid[y] == NULL) {
            for(int i = 0; i < y; i++) free(grid[i]);
            free(grid);
            stop();
            return;
        }
    }

    lerw();
    spawncheeses();

    found = 0; 

    cursor curs = {1, 1, {"@"}};

    running = 1;
    
    while (running) {
        int ch = getch();
        switch (ch) {
            case 'q': case 'Q': running = 0; break;
            case 'r': case 'R': properrestart(&curs, &W, &H); break;
            case 'a': case 'A': case KEY_LEFT:
                if(curs.posx > 0 && !iswall(curs.posy, curs.posx - 1)){
                    curs.posx -= 1;
                }
                    break;
            case 'd': case 'D': case KEY_RIGHT:
                if (!iswall(curs.posy, curs.posx + 1)){
                    curs.posx += 1;
                }
                    break;
            case 'w': case 'W': case KEY_UP:
                if (curs.posy > 0 && !iswall(curs.posy - 1, curs.posx)){
                    curs.posy -= 1;
                }
                    break; 
            case 's': case 'S': case KEY_DOWN:
                if (!iswall(curs.posy + 1, curs.posx)){
                    curs.posy += 1;
                }
                    break; 
        }
        
        eatcheese(&curs);
        checkexit(&curs);

        if (!running) break;
        werase(win);
        box(win, 0, 0);

        mvwprintw(win, 1, width/3+2, "move around the maze and find food!"); 
        mvwprintw(win, 2, width/3+2, "you've collected %d/5 cheeses", found); 
        mvwprintw(win, 3, width/3+2, "(only if you find food your rat will be less hungry)"); 
        mvwprintw(win, height/1.5-2+1, width/3+2, "btw sometimes the maze is impossible just press R");
        mvwprintw(win, height/1.5-1+1, width/3+2, "if that's the case");

        for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++){
        if (grid[y][x] == '#') mvwaddch(win, y+1, x+1, '#');
        else if (grid[y][x] == 'O') {
            int dist = abs(y - curs.posy) + abs(x - curs.posx);
            if (dist <= 5) mvwaddch(win, y+1, x+1, 'O');
            }
        }

        int wmsg = ((width/3) - 1) / 3;
        int hmsg = ((height/2) - 1) / 2;
        int msgw = 3 * wmsg + 1;
        int msgh = 3 * hmsg + 1;        
        mvwprintw(win, msgh+1, msgw-2+1, "^ leave through here");

        mvwaddstr(win, curs.posy+1, curs.posx+1, curs.face[0]);

        wrefresh(win);
        usleep(50000);
    }

    for (int y = 0; y < H; y++)
    free(grid[y]);
    free(grid);

stop(); 
}
