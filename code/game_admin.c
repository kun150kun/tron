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
#include <simpl.h>
#include <ncurses.h>
#include "message.h"

#define N 4

struct Node{
    ARENA arena;
    struct Node* next;
};

void enqueue(struct Node** head,struct Node** tail,ARENA arena){
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->arena = arena;
    temp->next = NULL;
    if(*head == NULL && *tail == NULL){
        *head = *tail = temp;
        return;
    }
    (*tail)->next = temp;
    *tail = temp;
}

void dequeue(struct Node** head,struct Node** tail){
    struct Node* temp = *head;
    if(*head == NULL) {
        return;
    }
    if(*head == *tail) {
        *head = *tail = NULL;
    }
    else {
        *head = (*head)->next;
    }
    free(temp);
}

ARENA front(struct Node* head,struct Node* tail){
    if(head != NULL)
        return head->arena;
    ARENA a;
    return a;
}

int reg(char* fromWhom,MESSAGE msg,int* id);
void Move(MESSAGE* pReply,int* boostNum,int* boostTime,int * turn);
void bufferReply(char** fromWhomArr, MESSAGE* msgArr,MESSAGE* pReply,struct Node** head,struct Node** tail);
void startReply(char** fromWhomArr, MESSAGE* msgArr,MESSAGE* pReply);
int ALLstart(int * id);
int AllEnd(int* id);

//FILE *f;

int main(){
    
    if(name_attach("Game_Admin",NULL) == -1){
        printf("can't not attach name in game admin\n");
        exit(1);
    }
    
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr,TRUE);
    keypad(stdscr,TRUE);
    curs_set(0);
    
    
    //message
    int i,j;
    char* fromWhom = NULL;
    MESSAGE msg,reply;
    char** fromWhomArr = (char **)malloc(N*sizeof(char*));
    MESSAGE* msgArr = (MESSAGE*)malloc(N*sizeof(MESSAGE));
    int turn = 0;
    int boostTime[2] = {0,0};
    struct Node* head = NULL;
    struct Node* tail = NULL;
    for(i = 0;i<N;i++){
        fromWhomArr[i] = NULL;
    }
    
    //init game.
    int W;
    getmaxyx(stdscr,i,W);
    int id[4] = {-1,-1,-1,-1};
    int boostNum[2] = {3,3};
    for(i = 1;i<MAX_WIDTH-1;i++){
        for(j =1;j<MAX_HEIGHT-1;j++){
            reply.arena.wall[i][j] = 0;
        }
    }
    for(i = 0;i<MAX_WIDTH;i++){
        reply.arena.wall[i][0] = 3;
        reply.arena.wall[i][MAX_HEIGHT - 1] = 3;
    }
    for(i = 1;i < MAX_HEIGHT - 1;i ++){
        reply.arena.wall[0][i] = 3;
        reply.arena.wall[MAX_WIDTH - 1][i] = 3;
    }
    reply.arena.cycle[0].pos.y = MAX_HEIGHT/2;
    reply.arena.cycle[0].pos.x = 100 - W/2.0 + 8;
    reply.arena.cycle[1].pos.y = MAX_HEIGHT/2;
    reply.arena.cycle[1].pos.x = 100 + W/2.0 - 9;
    reply.arena.cycle[0].dir = EAST;
    reply.arena.cycle[1].dir = WEST;
    reply.interval = 50;
    reply.boost = NO;
    
    //f = fopen("a.txt","w");
    while(1){
        if(Receive(&fromWhom, &msg, sizeof(msg)) == -1){
            printf("can't not receive in game admin\n");
            exit(1);
        }
        if(msg.type == REGISTER_TIMER || msg.type == REGISTER_CYCLE || msg.type == REGISTER_COURIER || msg.type == REGISTER_HUMAN){
            turn = reg(fromWhom,msg,id);
        }
        //timer
        else if(msg.type == TIMER_READY){
            if(turn == -1){
                id[2] = -2;
                reply.type = END;
                //fprintf(f,"timer end\n");
                if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                    printf("can't not reply in game admin\n");
                    exit(-1);
                }
                if(AllEnd(id)){
                    break;
                }
            }
            else if(turn == 2){
                reply.type = SLEEP;
                reply.interval = boostTime[0] + boostTime[1] > 0? 10000:50000;
                if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                    printf("can't not reply in game admin\n");
                    exit(-1);
                }
                Move(&reply,boostNum,boostTime,&turn);
                bufferReply(fromWhomArr, msgArr,&reply,&head,&tail);
            }
            else{
                id[2] = 1;
                fromWhomArr[2] = fromWhom;
                msgArr[2].type = TIMER_READY;
                if(ALLstart(id)){
                    turn = 2;
                    startReply(fromWhomArr,msgArr,&reply);
                }
            }
        }
        //cycle0,1
        else if(msg.type == CYCLE_READY){
            id[msg.cycleId] = 1;
            fromWhomArr[msg.cycleId] = fromWhom;
            msgArr[msg.cycleId].type = CYCLE_READY;
            if(ALLstart(id)){
                turn = 2;
                startReply(fromWhomArr,msgArr,&reply);
            }
        }
        else if(msg.type == MOVE){
            if(turn == -1){
                reply.type = END;
                id[msg.cycleId] = -2;
                //fprintf(f,"cycle end\n");
                if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                    printf("can't not reply in game admin\n");
                    exit(-1);
                }
                if(AllEnd(id)){
                    break;
                }
            }
            else{
                fromWhomArr[msg.cycleId] = fromWhom;
                msgArr[msg.cycleId].type = msg.type;
                if(reply.arena.cycle[msg.cycleId].dir - msg.dir != 2 || msg.dir - reply.arena.cycle[msg.cycleId].dir != 2)
                    reply.arena.cycle[msg.cycleId].dir = msg.dir;
                if(msg.boost == YES && boostNum[msg.cycleId] > 0 && boostTime[msg.cycleId] == 0){
                    boostNum[msg.cycleId] -= 1;
                    boostTime[msg.cycleId] = 5;
                }
            }
        }
        //human
        else if(msg.type == HUMAN_READY){
            id[msg.humanId] = 1;
            fromWhomArr[msg.humanId] = fromWhom;
            msgArr[msg.humanId].type = HUMAN_READY;
            if(ALLstart(id)){
                turn = 2;
                startReply(fromWhomArr,msgArr,&reply);
            }
        }
        else if(msg.type == HUMAN_MOVE){
            if(turn == -1){
                reply.type = END;
                //fprintf(f,"human end\n");
                id[msg.humanId] = -2;
                if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                    printf("can't not reply in game admin\n");
                    exit(-1);
                }
                if(AllEnd(id)){
                    break;
                }
            }
            else{
                fromWhomArr[msg.humanId] = fromWhom;
                msgArr[msg.humanId].type = msg.type;
                if(reply.arena.cycle[msg.humanId].dir - msg.dir != 2 && msg.dir - reply.arena.cycle[msg.humanId].dir != 2)
                    reply.arena.cycle[msg.humanId].dir = msg.dir;
                if(msg.boost == YES && boostNum[msg.humanId] > 0 && boostTime[msg.humanId] == 0){
                    boostNum[msg.humanId] -= 1;
                    boostTime[msg.humanId] = 5;
                }
            }
        }
        //courier2
        else if(msg.type == COURIER_READY){
            id[3] = 1;
            fromWhomArr[3] = fromWhom;
            msgArr[3].type = COURIER_READY;
            if(ALLstart(id)){
                turn = 2;
                startReply(fromWhomArr,msgArr,&reply);
            }
        }
        else if(msg.type == OKAY){
            if(turn == -1 && head == NULL){
                id[3] = -2;
                reply.type = END;
                //fprintf(f,"courier end\n");
                if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                    printf("can't not reply in game admin\n");
                    exit(-1);
                }
                if(AllEnd(id)){
                    break;
                }
            }
            else if(head != NULL){
                MESSAGE disreply;
                disreply.type = DISPLAY_ARENA;
                disreply.arena = front(head,tail);
                if(Reply(fromWhom, &disreply,sizeof(disreply)) == -1){
                    printf("can't not reply in game admin\n");
                    exit(-1);
                }
                dequeue(&head,&tail);
            }
            else{
                fromWhomArr[3] = fromWhom;
                msgArr[3].type = msg.type;
            }
        }
    }
    sleep(3);
    endwin();
    if(name_detach() == -1){
        printf("can't not detach name in game admin\n");
        exit(1);
    }
    //fprintf(f,"game_admin ends\n");
    return 0;
}

int reg(char* fromWhom,MESSAGE msg,int* id){
    MESSAGE reply;
    reply.type = INIT;
    
    if(msg.type == REGISTER_CYCLE){
        if(id[0] == -1){
            reply.cycleId = 0;
            id[0] = 0;
        }
        else if(id[1] == -1){
            reply.cycleId = 1;
            id[1] = 0;
        }
        else{
            reply.type = FAIL;
        }
    }
    else if(msg.type == REGISTER_HUMAN){
        if(id[0] == -1){
            reply.humanId = 0;
            id[0] = 0;
        }
        else if(id[1] == -1){
            reply.humanId = 1;
            id[1] = 0;
        }
        else{
            reply.type = FAIL;
        }
    }
    else if(msg.type == REGISTER_TIMER){
        if(id[2] == -1){
            id[2] = 0;
        }
        else{
            reply.type = FAIL;
        }
    }
    else if(msg.type == REGISTER_COURIER){
        if(id[3] == -1){
            id[3] = 0;
        }
        else{
            reply.type = FAIL;
        }
    }
    if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
        printf("can't not reply in game admin\n");
        exit(-1);
    }
    int i;
    for(i = 0;i < N;i++){
        if(id[i] == -1)
            return 0;
    }
    return 1;
}

void Move(MESSAGE* pReply,int* boostNum,int* boostTime,int * turn){
    int H,W;
    getmaxyx(stdscr,H,W);
    DIRECTION d;
    COORDINATE cur;
    int dx,dy;
    pReply->cycleId = -2;
    int i;
    if(boostTime[0] == boostTime[1] || (boostTime[0] + boostTime[1]) % 5 == 0){
        for(i = 0;i < 2;i++){
            cur.x = pReply->arena.cycle[i].pos.x;
            cur.y = pReply->arena.cycle[i].pos.y;
            d = pReply->arena.cycle[i].dir;
            
            if(d == NORTH){
                cur.y -= 1;
            }
            else if(d == SOUTH){
                cur.y += 1;
            }
            else if(d == WEST){
                cur.x -= 1;
            }
            else if(d == EAST){
                cur.x += 1;
            }
            dx = cur.x - pReply->arena.cycle[1-i].pos.x;
            dy = cur.y - pReply->arena.cycle[1-i].pos.y;
            if(dx <= W - 6 && dx >= 6 - W  && dy <= H - 6 && dy >= 6 - H){
                pReply->arena.wall[pReply->arena.cycle[i].pos.x][pReply->arena.cycle[i].pos.y] = i + 1;
                pReply->arena.cycle[i].pos.x = cur.x;
                pReply->arena.cycle[i].pos.y = cur.y;
            }
            if(boostTime[i] > 0)
                boostTime[i] -= 1;
        }
    }
    else{
        for(i = 0;i < 2;i++){
            if(boostTime[i] == 0 || boostTime[i] == 5)
                continue;
            cur.x = pReply->arena.cycle[i].pos.x;
            cur.y = pReply->arena.cycle[i].pos.y;
            d = pReply->arena.cycle[i].dir;
            
            if(d == NORTH){
                cur.y -= 1;
            }
            else if(d == SOUTH){
                cur.y += 1;
            }
            else if(d == WEST){
                cur.x -= 1;
            }
            else if(d == EAST){
                cur.x += 1;
            }
            dx = cur.x - pReply->arena.cycle[1-i].pos.x;
            dy = cur.y - pReply->arena.cycle[1-i].pos.y;
            if(dx <= W - 6 && dx >= 6 - W  && dy <= H - 6 && dy >= 6 - H){
                pReply->arena.wall[pReply->arena.cycle[i].pos.x][pReply->arena.cycle[i].pos.y] = i + 1;
                pReply->arena.cycle[i].pos.x = cur.x;
                pReply->arena.cycle[i].pos.y = cur.y;
            }
            if(boostTime[i] > 0)
                boostTime[i] -= 1;
        }
    }
    for(i = 0;i < 2;i++){
        if(pReply->arena.wall[pReply->arena.cycle[i].pos.x][pReply->arena.cycle[i].pos.y] != NONE){
            *turn = -1;
            if(pReply->cycleId == i)
                pReply->cycleId = -1;
            else
                pReply->cycleId = 1 - i;
        }
    }
    if(pReply->arena.cycle[0].pos.x == pReply->arena.cycle[1].pos.x && pReply->arena.cycle[0].pos.y == pReply->arena.cycle[1].pos.y){
        pReply->cycleId = -1;
        *turn = -1;
    }
}

void bufferReply(char** fromWhomArr, MESSAGE* msgArr,MESSAGE* pReply,struct Node** head,struct Node** tail){
    int id = pReply->cycleId;
    if(fromWhomArr[3] != NULL && msgArr[3].type == OKAY){
        pReply->type = DISPLAY_ARENA;
        if(Reply(fromWhomArr[3], pReply,sizeof(*pReply)) == -1){
            printf("can't not reply in game admin\n");
            exit(1);
        }
        fromWhomArr[3] = NULL;
    }
    else{
        enqueue(head,tail,pReply->arena);
    }
    int i;
    for(i = 0;i < 2;i ++){
        if(fromWhomArr[i] != NULL && (msgArr[i].type == MOVE || msgArr[i].type == HUMAN_MOVE)){
            pReply->type = UPDATE;
            pReply->cycleId = i;
            pReply->humanId = i;
            if(Reply(fromWhomArr[i], pReply,sizeof(*pReply)) == -1){
                printf("can't not reply in game admin\n");
                exit(1);
            }
            fromWhomArr[i] = NULL;
        }
    }
    pReply->cycleId = id;
}

void startReply(char** fromWhomArr, MESSAGE* msgArr,MESSAGE* pReply){
    int i;
    pReply->type = START;
    for(i = 0;i < 2;i++){
        if(msgArr[i].type == HUMAN_READY){
            pReply->humanId = i;
        }
        else{
            pReply->cycleId = i;
        }
        if(Reply(fromWhomArr[i], pReply,sizeof(*pReply)) == -1){
            printf("can't not reply in game admin\n");
            exit(1);
        }
        fromWhomArr[i] = NULL;
    }
    pReply->type = SLEEP;
    pReply->interval = 50;
    if(Reply(fromWhomArr[2], pReply,sizeof(*pReply)) == -1){
        printf("can't not reply in game admin\n");
        exit(1);
    }
    pReply->type = DISPLAY_ARENA;
    if(Reply(fromWhomArr[3], pReply,sizeof(*pReply)) == -1){
        printf("can't not reply in game admin\n");
        exit(1);
    }
    fromWhomArr[3] = NULL;
}

int AllEnd(int* id){
    int i;
    for(i = 0;i < N;i++){
        if(id[i] != -2){
            return 0;
        }
    }
    return 1;
}

int ALLstart(int * id){
    int i;
    for(i = 0;i < N;i++){
        if(id[i] != 1){
            return 0;
        }
    }
    return 1;
}
