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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <simpl.h>
#include "message.h"

int input(int argc, char *argv[],char *name);
void Courier2(int fd);
void Courier01(int fd,int numOfCourier);

int main(int argc, char *argv[]){
    char* name = (char *)malloc(sizeof(char)*9);
    int numOfCourier = input(argc,argv,name);
    int fd;
    if(name_attach(name,NULL) == -1){
        printf("can't not attach name in %s\n",name);
        exit(1);
    }
    if((fd = name_locate("Game_Admin")) == -1){
        printf("can't not locate game_admin in courier\n");
        exit(1);
    }
    
    if(numOfCourier == 2){
        
        Courier2(fd);
    }
    else{
        Courier01(fd,numOfCourier);
    }
    
    if(name_detach() == -1){
        printf("can't not detach name in courier\n");
        exit(1);
    }
    free(name);
    return 0;
}

int input(int argc, char *argv[],char *name){
    char *numOfCourier;
    if(argc == 2){
        numOfCourier = argv[1];
        if(strcmp(numOfCourier,"0") != 0 && strcmp(numOfCourier,"1") != 0 && strcmp(numOfCourier,"2") != 0)
            exit(-1);
    }
    else
        exit(-1);
    strcpy(name,"Courier");
    strcat(name,numOfCourier);
    return atoi(numOfCourier);
}

void Courier2(int fd){
    int fd2;
    MESSAGE msg,reply;
    if((fd2 = name_locate("Display_Admin")) == -1){
        printf("can't not locate display_admin in courier2\n");
        exit(1);
    }
    
    msg.type = REGISTER_COURIER;
    if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
        printf("can't not send in courier2\n");
        exit(1);
    }
    if(reply.type != INIT && reply.type != FAIL){
        
        exit(1);
    }
    
    if(reply.type == INIT){
        msg.type = COURIER_READY;
        while(1){
            if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
                printf("can't not send in courier2\n");
                exit(1);
            }
            if(Send(fd2, &reply, &msg, sizeof(reply), sizeof(msg)) == -1){
                printf("can't not send in courier2\n");
                exit(1);
            }
            
            if(reply.type == END){
                
                return;
            }
            else if(reply.type != DISPLAY_ARENA){
                exit(1);
            }
        }
    }
}

void Courier01(int fd,int numOfCourier){
    int fd2;
    MESSAGE msg,reply;
    if((fd2 = name_locate("Input_Admin")) == -1){
        printf("can't not locate input_admin in courier01\n");
        exit(1);
    }
    
    msg.type = REGISTER_COURIER;
    if(Send(fd2, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
        printf("can't not send in courier01\n");
        exit(1);
    }
    if(reply.type != INIT && reply.type != FAIL){
        exit(1);
    }
    
    if(reply.type != INIT){
        return;
    }
    
    msg.type = COURIER_READY;
    if(Send(fd2, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
        printf("can't not send in courier01\n");
        exit(1);
    }
    if(reply.type != REGISTER_HUMAN){
        return;
    }
    while(1){
        //register
        if(Send(fd, &reply, &msg, sizeof(reply), sizeof(msg)) == -1){
            printf("can't not send in courier01\n");
            exit(1);
        }
        
        if(Send(fd2, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
            printf("can't not send in courier01\n");
            exit(1);
        }
        if(msg.type == FAIL || msg.type == END){
            return;
        }
    }
    
}
