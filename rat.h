#ifndef rat_h
#define rat_h

#include <ncurses.h>

typedef struct {
    float hunger;
    float love;
    float fun;
    float clean;
    float health;
    int age;
} Rat;

extern char *ratart[];
extern char *funfact[]; 
extern char *messages[];

extern Rat rat;

void load(void);
void save(void);
void setstat(void);
void addstat(float *guy);

WINDOW *ratdrawbox(void);
int wrapprint(WINDOW *win, int row, int col, int maxwidth, const char *msg);

void gameguess(void);
void gamecatch(void);
void gamemaze(void);
void gameclicker(void);

#endif
