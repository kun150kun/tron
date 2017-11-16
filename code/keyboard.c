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

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <simpl.h>
#include "message.h"


int main(){
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr,TRUE);
    keypad(stdscr,TRUE);
    int fd;
    MESSAGE msg,reply;
    if(name_attach("Keyboard",NULL) == -1){
        printf("can't not attach name in keyboard\n");
        exit(1);
    }
    if((fd = name_locate("Input_Admin")) == -1){
        printf("can't not locate input_admin in keyboard\n");
        exit(1);
    }
    
    msg.type = REGISTER_KEYBOARD;
    if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1 ){
        printf("can't not send in keyboard\n");
        exit(1);
    }
    if(reply.type != INIT){
        return 0;
    }
    msg.type = KEYBOARD_READY;
    if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
        printf("can't not send in keyboard\n");
        exit(1);
    }
    if(reply.type != START){
        return 0;
    }
    msg.type = KEYBOARD_INPUT;
    while (1) {
        int i;
        for(i = 0;i < MAX_KEYS;i++){
            msg.key[i] = getch();
        }
        if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
            printf("can't not send in keyboard\n");
            exit(1);
        }
        if(reply.type == END){
            break;
        }
        else if(reply.type == OKAY){
            continue;
        }
        else{
            exit(1);
        }
    }
    
    sleep(3);
    endwin();
    if(name_detach() == -1){
        printf("can't not detach name in keyboard\n");
        exit(1);
    }
    return 0;
}
