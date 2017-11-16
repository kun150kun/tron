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
#include <simpl.h>
#include "message.h"
#include <stdlib.h>

int main(){
    int fd;
    MESSAGE msg,reply;
    if(name_attach("Timer",NULL) == -1){
        printf("can't not attach name in timer\n");
        exit(1);
    }
    if((fd = name_locate("Game_Admin")) == -1){
        printf("can't not locate game_admin in timer\n");
        exit(1);
    }
    msg.type = REGISTER_TIMER;
    if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1 ){
        printf("can't not send in timer\n");
        exit(1);
    }
    if(reply.type != INIT && reply.type != FAIL){
        exit(0);
    }
    
    if(reply.type == INIT){
        msg.type = TIMER_READY;
        while(1){
            if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
                printf("can't not send in timer\n");
                exit(1);
            }
            if(reply.type == END){
                break;
            }
            else if(reply.type == SLEEP){
                usleep(reply.interval);
            }
            else{
                exit(1);
            }
        }
    }
    
    if(name_detach() == -1){
        printf("can't not detach name in timer\n");
        exit(1);
    }
    return 0;
}
