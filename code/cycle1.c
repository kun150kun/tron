#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include <simpl.h>
#include <time.h>

void input(int argc, char *argv[],char *name);
int nextMove(MESSAGE* reply,int cycleId,int depth);


int main(int argc, char *argv[]){
    srand(time(NULL));
    char* name = (char *)malloc(sizeof(char)*7);
    input(argc,argv,name);
    int fd;
    MESSAGE msg,reply;
    
    if(name_attach(name,NULL) == -1){
        printf("can't not attach name in %s\n",name);
        exit(1);
    }
    if((fd = name_locate("Game_Admin")) == -1){
        printf("can't not locate game_admin in cycle\n");
        exit(1);
    }
    
    msg.type = REGISTER_CYCLE;
    if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1 ){
        printf("can't not send in cycle\n");
        exit(1);
    }
    if(reply.type != INIT && reply.type != FAIL){
        exit(0);
    }
    if(reply.type == INIT){
        msg.type = CYCLE_READY;
        msg.cycleId = reply.cycleId;
        if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
            printf("can't not send in cycle\n");
            exit(1);
        }
        if(reply.type != START){
            exit(0);
        }
    }
    
    if(reply.type == START){
        msg.type = MOVE;
        while(1){
            msg.boost = NO;
            msg.dir = nextMove(&reply,msg.cycleId,1);
            if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
                printf("can't not send in cycle\n");
                exit(1);
            }
            if(reply.type == END){
                break;
            }
            else if(reply.type != UPDATE){
                exit(0);
            }
        }
    }
    free(name);
    if(name_detach() == -1){
        printf("can't not detach name in %s\n",name);
        exit(1);
    }
    return 0;
}

void input(int argc, char *argv[],char *name){
    char *numOfCycle;
    if(argc == 2){
        numOfCycle = argv[1];
        if(strcmp(numOfCycle,"0") != 0 && strcmp(numOfCycle,"1") != 0)
            exit(2);
    }
    else
        exit(2);
    strcpy(name,"Cycle");
    strcat(name,numOfCycle);
}

//ai...
int nextMove(MESSAGE* reply,int cycleId,int depth){
    int num = 0;
    DIRECTION nextDir = 0;
    COORDINATE p1 = reply->arena.cycle[cycleId].pos;
    COORDINATE p2 = reply->arena.cycle[1-cycleId].pos;
    int arr[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    int j;
    for(j = 0;j < 4;j++){
        if((p1.x + arr[j][0] != p2.x || p1.y + arr[j][1] != p2.y) && reply->arena.wall[p1.x + arr[j][0]][p1.y + arr[j][1]] == NONE){
            num++;
            if(cycleId == 1)    rand();
            if(num == 1){
                nextDir = j;
            }
            else if(rand()%num == 0){
                nextDir = j;
            }
        }
    }
    if(cycleId == 1)    rand();
    if(num == 0)
        return rand() % 5;
    return nextDir;
}
