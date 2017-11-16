/*
 * CSCI3180 Principles of Programming Languages
 *
 * --- Declaration ---
 *
 * I declare that the assignment here submitted is original except for source
 * material explicitly acknowledged. I also acknowledge that I am aware of
 * University policy and regulations on honesty in academic work, and of the
 * disciplinary guidelines and procedures applicable to breaches of such policy
 * and regulations, as contained in the website
 * http://www.cuhk.edu.hk/policy/academichonesty/
 *
 * Assignment 5B
 * Name : Huang Kun Ping
 * Student ID : 1155092194
 * Email Addr : kphuang6@cse.cuhk.edu.hk
 *
 */

#include <sys/types.h>
#include <ncurses.h>
#include "message.h"
#include <simpl.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void printWin(int cycleId);
void printScreen(MESSAGE* reply,int win);

int main(){
    initscr();
    start_color();
    cbreak();
    noecho();
    nodelay(stdscr,TRUE);
    keypad(stdscr,TRUE);
    curs_set(0);
    
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    
    init_pair(3, COLOR_BLACK,COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_RED);
    init_pair(5, COLOR_GREEN, COLOR_GREEN);
    init_pair(6, COLOR_WHITE,COLOR_WHITE);
    
    int fd;
    MESSAGE msg,reply;
    if(name_attach("Painter",NULL) == -1){
        printf("can't not attach name in painter\n");
        exit(1);
    }
    if((fd = name_locate("Display_Admin")) == -1){
        printf("can't not locate display_admin in painter\n");
        exit(1);
    }
    
    msg.type = PAINTER_READY;
    while(1){
        if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
            printf("can't not send in painter\n");
            exit(1);
        }
        
        if(reply.type == END){
            clear();
            printScreen(&reply,1);
            printWin(reply.cycleId);
            break;
        }
        else if(reply.type == PAINT){
            clear();
            printScreen(&reply,0);
        }
        else{
            exit(1);
        }
    }
    
    sleep(3);
    
    if(name_detach() == -1){
        printf("can't not detach name in painter\n");
        exit(1);
    }
    endwin();
    return 0;
}


void printWin(int cycleId){
    attrset(A_NORMAL);
    if(cycleId == -1){
        mvprintw(0,0,"*               DRAW!                *");
    }
    else if(cycleId == 1){
        mvprintw(0,0,"*        GREEN wins the game!        *");
    }
    else{
        mvprintw(0,0,"*         RED wins the game!         *");
    }
    refresh();
}

void printScreen(MESSAGE* reply,int win){
    int H,W;
    getmaxyx(stdscr,H,W);
    COORDINATE Pos1,Pos2;
    Pos1 = reply->arena.cycle[0].pos;
    Pos2 = reply->arena.cycle[1].pos;
    COORDINATE init;
    
    
    
    init.x = (Pos1.x + Pos2.x + 1)/2 - W/2;
    init.y = (Pos1.y + Pos2.y + 1)/2 - H/2;
    int i,j,color;
    
    for(j = 0;j < H;j++){
        for(i = 0;i < W;i++){
            if(init.x + i < 0 || init.y + j < 0 || init.x + i >= MAX_WIDTH || init.y + j >= MAX_HEIGHT){
                attron(A_REVERSE | COLOR_PAIR(3));
                mvaddch(j,i,' ');
                attroff(A_REVERSE | COLOR_PAIR(3));
                continue;
            }
            color = reply->arena.wall[init.x + i][init.y + j];
            attron(A_REVERSE | COLOR_PAIR(color + 3));
            mvaddch(j,i,' ');
            attroff(A_REVERSE | COLOR_PAIR(color + 3));
        }
    }
    
    if(win == 0 || (win == 1 && reply->cycleId == 0)){
        attron(COLOR_PAIR(1));
        mvaddch(Pos1.y-init.y,Pos1.x-init.x,'@');
        attroff(COLOR_PAIR(1));
    }
    if(win == 0 || (win == 1 && reply->cycleId == 1)){
        attron(COLOR_PAIR(2));
        mvaddch(Pos2.y-init.y,Pos2.x-init.x,'@');
        attroff(COLOR_PAIR(2));
    }
    refresh();
}
