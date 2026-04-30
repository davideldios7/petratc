#ifndef rat_h
#define rat_h

typedef struct {
    float hunger;
    float love;
    float fun;
    float clean;
    float health;
    int age;
} Rat;

extern char ratart[];
extern char *messages[];
extern Rat rat;
void addstat(float *guy);

#endif
