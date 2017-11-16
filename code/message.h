#ifndef __MESSAGE_H
#define __MESSAGE_H

/* Maximum number of keys to read in from the keyboard at once */
#define MAX_KEYS       8
#define MAX_CYCLE	   2
#define MAX_WIDTH	   200
#define MAX_HEIGHT	   100

/* Message Token */
typedef enum {
    REGISTER_CYCLE, INIT, FAIL, CYCLE_READY, START, MOVE, UPDATE, END,//0-7
    REGISTER_TIMER, TIMER_READY, SLEEP,//8-10
    REGISTER_HUMAN, HUMAN_READY, HUMAN_MOVE,//11-13
    REGISTER_KEYBOARD, KEYBOARD_READY, KEYBOARD_INPUT,//14-16
    REGISTER_COURIER, COURIER_READY,//17,18
    DISPLAY_ARENA, OKAY, PAINTER_READY, PAINT//19-22
} MESSAGE_TYPE;

/* Direction */
typedef enum {
    EAST = 0, SOUTH = 1, WEST = 2, NORTH = 3
} DIRECTION;

/* Coordinate */
typedef struct {
    int y,x;
} COORDINATE;

/* Cycle */
typedef struct {
    COORDINATE pos;
    DIRECTION dir;
}CYCLE;

/* Wall */
typedef enum {
    NONE = 0, REDCYCLE = 1, GREENCYCLE = 2, BOUNDARY = 3
}WALL;

/* Arena */
typedef struct {
    WALL wall[MAX_WIDTH][MAX_HEIGHT];
    CYCLE cycle[MAX_CYCLE];
} ARENA;

/* Booster */
typedef enum {
    YES = 0, NO = 1
} BOOST;

/* Message Format */
typedef struct {
    
    MESSAGE_TYPE type;  /* Message Type */
    int cycleId;        /* Cycle ID */
    int interval;       /* Sleep Interval */
    DIRECTION dir;		/* Cycle move direction */
    BOOST boost;		/* Cycle use booster? */
    ARENA arena;        /* Current Game State */
    
    /* Message field for the bonus part */
    int humanId;           /* Courier ID*/
    int key[MAX_KEYS];		/* List of key ASCIIs*/
    
} MESSAGE;

#endif
