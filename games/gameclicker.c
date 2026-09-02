#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/stat.h>
#include "../rat.h"

/*
i lowkey stole the art of the little rat and the cheese from the internet lol
https://asciiart.website/art/752
*/

static int running;


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

typedef struct ratworker{
    char *name; 
    bignum price;
    ////line 67 six seveeennn
    bignum clicks; 
    bignum multiplier; //the more you buy the more expensive the unit becomes
    bignum owned;
}ratworker;

typedef struct{
    char *name;
    bignum price;
    bignum multiplier;
    char *whichstat;  
    int howmanystat;  //how many times itc alls addstat() for that stat
}prizes;  

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

/*
 * returns 1 if a is larger
 * returns 0 if same number
 * returns -1 is a is tiny
*/
static int comparebignum(bignum a, bignum b){
    //branch predictors will hate this function :sob:
    if(a.y == b.y){
        if(a.x == b.x){
            return 0;
            }else if(a.x > b.x){
                return 1;
            }else return -1;
        }else if(a.y > b.y){ 
            return 1;
    } else return -1; 
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

/* clamps a bignum to zero if it somehow goes negative */
static bignum clampnonneg(bignum b){
    if(b.x < 0.0 || b.y < 0) return (bignum){0.0, 0};
    return b;
}

static bignum calculateworkerincome(ratworker worker){
    return mulbignum(worker.clicks, worker.owned);
}

static bignum calculatetotalincome(ratworker workers[], int n){
    bignum total = {0.0, 0}; 
    for(int i = 0; i < n; i++){
        total = addbignum(total, calculateworkerincome(workers[i]));
    } 
    return total;  
} 

static int buyworker(bignum *cheeses, ratworker *worker){
    if(comparebignum(*cheeses, worker->price) < 0) return 0; //nigga broke

    *cheeses = subbignum(*cheeses, worker->price);
    *cheeses = clampnonneg(*cheeses);
    worker->owned = addmixregbig(worker->owned, 1.0);
    worker->price = mulbignum(worker->price, worker->multiplier);

    return 1;
}

static int buyprize(bignum *cheeses, prizes *prize){
    if(comparebignum(*cheeses, prize->price) < 0) return 0;

    *cheeses = subbignum(*cheeses, prize->price);
    *cheeses = clampnonneg(*cheeses);

    prize->price = mulbignum(prize->price, prize->multiplier);

    float *stat = NULL;
    if(strcmp(prize->whichstat, "hunger") == 0)      stat = &rat.hunger;
    else if(strcmp(prize->whichstat, "health") == 0) stat = &rat.health;
    else if(strcmp(prize->whichstat, "love") == 0)   stat = &rat.love;
    else if(strcmp(prize->whichstat, "fun") == 0)    stat = &rat.fun;
    else if(strcmp(prize->whichstat, "clean") == 0)  stat = &rat.clean;

    if(stat){
        for(int i = 0; i < prize->howmanystat; i++){
            addstat(stat);
        }
    }

    return 1;
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

static void drawwin(WINDOW *win, bignum cheeses, ratworker workers[], int n){
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
        scorecol = winw / 2 - 25;
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
    if(scorestr != NULL) {
        snprintf(scoremsg, sizeof(scoremsg), "cheese: %s", scorestr);
        free(scorestr);
    } else {
        snprintf(scoremsg, sizeof(scoremsg), "cheese: ERROR");
    }    
    char *incomestr = bignumprint(calculatetotalincome(workers, n));
    char incomemsg[64];
    if(incomestr != NULL) {
        snprintf(incomemsg, sizeof(incomemsg), "income/s: %s", incomestr);
        free(incomestr);
    } else {
        snprintf(incomemsg, sizeof(incomemsg), "income/s: ERROR");
    }

    if(mode == layoutwide){
        mvwprintw(win, 1, scorecol, "%s", scoremsg);
        mvwprintw(win, 2, scorecol, "%s", incomemsg);
        wrapprint(win, 3, scorecol, scorewrapw, "click the cheese!");;
    } else {
        int scorerow = artrow0 + artrows + 1;
        wrapprint(win, scorerow,   scorecol, scorewrapw, scoremsg);
        wrapprint(win, scorerow+1, scorecol, scorewrapw, incomemsg);
        wrapprint(win, scorerow+2, scorecol, scorewrapw, "click the cheese!");
    }
}

#define shopwidth 45
static void drawshop(WINDOW *win, ratworker workers[], int n, int selected, bignum cheeses){
    int winh = getmaxy(win);
    int winw = getmaxx(win);

    int shoph = n + 2;
    if(shoph > winh - 2) shoph = winh - 2; 
    int shopw = shopwidth;
    if(shopw > winw - 4) shopw = winw - 4;
    if(shoph < 3 || shopw < 10) return; 
    //if that skip it

    int shopy;
    if(getlayout(win) == layoutwide){
        shopy = 1;
    }else shopy = winh - shoph - 1;

    int shopx = winw - shopw - 2;

    mvwhline(win, shopy, shopx + 1, 0, shopw - 2);
    mvwhline(win, shopy + shoph - 1, shopx + 1, 0, shopw - 2);
    mvwvline(win, shopy + 1, shopx, 0, shoph - 2);
    mvwvline(win, shopy + 1, shopx + shopw - 1, 0, shoph - 2);
    mvwaddch(win, shopy, shopx, ACS_ULCORNER);
    mvwaddch(win, shopy, shopx + shopw - 1, ACS_URCORNER);
    mvwaddch(win, shopy + shoph - 1, shopx, ACS_LLCORNER);
    mvwaddch(win, shopy + shoph - 1, shopx + shopw - 1, ACS_LRCORNER);
    mvwprintw(win, shopy, shopx + 2, " shop (up/down, enter) ");

    int availw = shopw - 2;
    int visible = shoph - 2;
    int start = 0;
    if(selected >= visible) start = selected - visible + 1;
    if(start > n - visible) start = n - visible;
    if(start < 0) start = 0;

    for(int i = start; i < start + visible && i < n; i++){
        ratworker *w = &workers[i];
        char *pricestr = bignumprint(w->price);
        char *ownedstr = bignumprint(w->owned);
        char rightstr[32];
        snprintf(rightstr, sizeof(rightstr), "$%s x%s",
            pricestr ? pricestr : "?", ownedstr ? ownedstr : "?");        if(pricestr) free(pricestr);
        if(ownedstr) free(ownedstr);

        int rightlen = strlen(rightstr);
        int namewidth = availw - rightlen - 1; //-1 for a gap column
        if(namewidth < 0) namewidth = 0;

        char row[64];
        //fill the whole width first so shrinking a wider row doesn't leave weird gaps
        snprintf(row, sizeof(row), "%-*.*s", availw, availw, "");

        int affordable = comparebignum(cheeses, w->price) >= 0;
        if(i == selected) wattron(win, A_REVERSE);
        else if(!affordable) wattron(win, A_DIM);

        mvwprintw(win, shopy + 1 + (i - start), shopx + 1, "%s", row);
        mvwprintw(win, shopy + 1 + (i - start), shopx + 1, "%-*.*s", namewidth, namewidth, w->name);
        mvwprintw(win, shopy + 1 + (i - start), shopx + 1 + availw - rightlen, "%s", rightstr);

        if(i == selected) wattroff(win, A_REVERSE);
        else if(!affordable) wattroff(win, A_DIM);
    }//bwehghg
}

//i guess i will use this in a game... and i have plans
//for another game that will also require saving hehe
#ifdef __APPLE__
  #define clickerpath     "%s/Library/Application Support/rat"
  #define clickersavepath "%s/Library/Application Support/rat/clicker.txt"
#else
  #define clickerpath     "%s/.local/share/rat"
  #define clickersavepath "%s/.local/share/rat/clicker.txt"
#endif


static void clickersave(bignum cheeses, ratworker workers[], int n){
    char path[256];
    snprintf(path, sizeof(path), clickerpath, getenv("HOME"));
    mkdir(path, 0755);
    snprintf(path, sizeof(path), clickersavepath, getenv("HOME"));
    FILE *f = fopen(path, "w");
    if(!f) return;
    fprintf(f, "%lf\n%d\n", cheeses.x, cheeses.y);
    for(int i = 0; i < n; i++){
        fprintf(f, "%lf\n%d\n%lf\n%d\n",
            workers[i].price.x, workers[i].price.y,
            workers[i].owned.x, workers[i].owned.y);
    }
    fclose(f);
}

static void clickerload(bignum *cheeses, ratworker workers[], int n){
    char path[256];
    snprintf(path, sizeof(path), clickersavepath, getenv("HOME"));

    FILE *f = fopen(path, "r");
    if(!f) return; 

    if(fscanf(f, "%lf\n%d\n", &cheeses->x, &cheeses->y) != 2){
        fprintf(stderr, "error: failed to load clicker save data.\n");
        fclose(f);
        return; 
    }  
    for(int i = 0; i < n; i++){
        if(fscanf(f, "%lf\n%d\n%lf\n%d\n",
            &workers[i].price.x, &workers[i].price.y,
            &workers[i].owned.x, &workers[i].owned.y) != 4){
            fprintf(stderr, "error: failed to load clicker save data.\n");
            break;
        }
    }
    fclose(f);
}
//i should make a version of this that just saves a big string 
//so i can use many 
//like generate the "save file" that returns a pointer to a char
//and then propersave(*thatchar, "file.txt") and it makes file.txt 
//and it saves *thatchar to it idk this seems like a job for claude





static void draweverything(WINDOW *win, bignum cheeses, ratworker workers[], int n, int selected){
    drawwin(win, cheeses, workers, n);
    drawshop(win, workers, n, selected, cheeses);
    wrefresh(win);
}


#define spacedebouncems 20 

static long nowms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

void gameclicker(){

    srand(time(NULL));

    WINDOW *win = ratdrawbox();
    mousemask(BUTTON1_PRESSED, NULL);
    mouseinterval(0); //don't wait to resolve single vs double click

    running = 1;

    bignum cheeses = {0.0, 0};
    bignum onecheese = {1.0, 0}; //what a single click is worth
    int selected = 0;

#define nhowmanytiers 11
static ratworker workers[nhowmanytiers] = {
    //name,                      bignum price,clicks/tick,multiplier,owned (howmanyowned)
    { "small rat",                 {10.0, 0},  {1.0, 0},   {1.15, 0},  {0.0, 0} },
    { "brown rat",                 {20.0, 0},  {2.0, 0},   {1.15, 0},  {0.0, 0} },
    { "fancy rat",                 {25.0, 0},  {4.0, 0},   {1.14, 0},  {0.0, 0} },
    { "fat rat",                   {130.0, 0}, {10.0, 0},  {1.13, 0},  {0.0, 0} },
    { "evil rat",                  {400.0, 0}, {15.0, 0},  {1.13, 0},  {0.0, 0} },
    { "rat farm",                  {1.2, 1},   {25.0, 0},  {1.12, 0},  {0.0, 0} },
    { "cheese mill",               {3.0, 1},   {30.0, 0},  {1.11, 0},  {0.0, 0} },
    { "dairy factory",             {9.0, 1},   {50.0, 0},  {1.11, 0},  {0.0, 0} },
    { "cheese research institute", {9.5, 1},   {80.0, 0},  {1.05, 0},  {0.0, 0} },
    { "cheese accelerator",        {30.0, 1},  {100.0, 0}, {1.11, 0},  {0.0, 0} },
    { "the cheese singularity",    {120.0, 1}, {250.0, 0}, {1.10, 0},  {0.0, 0} },
};

#define nhowmanyprizes 10
static prizes prize[nhowmanyprizes] = {
    {"solid cheese",       {100.0, 0}, {1.15, 0}, "hunger", 2},
    {"cheese mountain",   {5000.0, 0}, {1.15, 0}, "hunger", 8},
    {"cheese wheel",       {500.0, 0}, {1.15, 0}, "health", 1}, 
    {"medical cheese",    {8000.0, 0}, {1.15, 0}, "health", 5},
    {"cheese fountain",   {2000.0, 0}, {1.15, 0}, "love",   3}, 
    {"love cheese cake", {15000.0, 0}, {1.15, 0}, "love",   7},
    {"squeaky cheese",     {300.0, 0}, {1.15, 0}, "fun",    2},
    {"party cheese",     {12000.0, 0}, {1.15, 0}, "fun",    6},
    {"soap cheese",        {400.0, 0}, {1.15, 0}, "clean",  2},
    {"bubble cheese",     {9000.0, 0}, {1.15, 0}, "clean",  5}    
}; 


    
    clickerload(&cheeses, workers, nhowmanytiers); 
    
    draweverything(win, cheeses, workers, nhowmanytiers, selected); 
    
    time_t lasttime = time(NULL);
    static int spaceheld = 0;
    static long lastspacetime = 0;
    while(running){

        int ch;
        while((ch = getch()) != ERR){ //drain the whole queue each frame so rapid clicks/keys aren't ignroed 
            switch (ch) {
                case 'q': case 'Q': running = 0; break;
                case KEY_UP: if(selected > 0) selected--; break;
                case KEY_DOWN: if(selected < nhowmanytiers - 1) selected++; break;
                case '\n': case KEY_ENTER:
                    buyworker(&cheeses, &workers[selected]);
                    break;
                case ' ': {
                    long now = nowms();
                    if(!spaceheld){
                    cheeses = addbignum(cheeses, onecheese); 
                    spaceheld = 1;
                        } 
                    lastspacetime = now;
                    break;
                    } 
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

        if(spaceheld && (nowms() - lastspacetime) > spacedebouncems){
          spaceheld = 0;
        }

        time_t currenttime = time(NULL);
        if(currenttime >= lasttime +1){
            cheeses = addbignum(cheeses, calculatetotalincome(workers, nhowmanytiers));
            cheeses = clampnonneg(cheeses);
            lasttime = currenttime;  
        }

        if(running) draweverything(win, cheeses, workers, nhowmanytiers, selected);

        usleep(16667); //60 fps according to google ai overview lmao
    }
clickersave(cheeses, workers, nhowmanytiers);
stop();
}
