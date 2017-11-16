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
#include "message.h"
#include <simpl.h>

//FILE * f;

struct Node{
    COORDINATE p;
    struct Node* next;
};

void enqueue(struct Node** head,struct Node** tail,COORDINATE p){
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->p = p;
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

COORDINATE front(struct Node* head){
    if(head != NULL)
        return head->p;
    COORDINATE p;
    return p;
}

void input(int argc, char *argv[],char *name);
int nextMove(MESSAGE* reply,int cycleId,int depth);
void Move(ARENA* arena,DIRECTION d,int i);
void unMove(ARENA* arena,DIRECTION d,int i);
void MaxAct(ARENA* arena, int cycleId, int alpha, int beta, int depth, int * score_dir);
void MinAct(ARENA* arena, int cycleId, int alpha, int beta, int depth, int * score_dir);
int score(ARENA* arena,int cycleId);

int main(int argc, char *argv[]){
    //f = fopen("a.txt","w");
    char* name = (char *)malloc(sizeof(char)*7);
    input(argc,argv,name);
    int fd;
    MESSAGE msg,reply;
    
    if(name_attach(name,NULL) == -1){
        exit(1);
    }
    if((fd = name_locate("Game_Admin")) == -1){
        exit(1);
    }
    
    msg.type = REGISTER_CYCLE;
    if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1 ){
        exit(1);
    }
    if(reply.type != INIT && reply.type != FAIL){
        exit(1);
    }
    if(reply.type == INIT){
        msg.type = CYCLE_READY;
        msg.cycleId = reply.cycleId;
        if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
            exit(1);
        }
        if(reply.type != START){
            exit(1);
        }
    }
    
    if(reply.type == START){
        msg.type = MOVE;
        while(1){
            msg.boost = NO;
            msg.dir = nextMove(&reply,msg.cycleId, 2);
            
            if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
                exit(1);
            }
            if(reply.type == END){
                break;
            }
            else if(reply.type != UPDATE){
                exit(1);
            }
        }
    }
    free(name);
    if(name_detach() == -1){
        exit(1);
    }
//    fclose(f);
    return 0;
}

void input(int argc, char *argv[],char *name){
    char *numOfCycle;
    if(argc == 2){
        numOfCycle = argv[1];
        if(strcmp(numOfCycle,"0") != 0 && strcmp(numOfCycle,"1") != 0)
            exit(-1);
    }
    else
        exit(-1);
    strcpy(name,"Cycle");
    strcat(name,numOfCycle);
}
//ai...
int score(ARENA* arena,int cycleId){
    COORDINATE p1 = arena->cycle[cycleId].pos;
    COORDINATE p2 = arena->cycle[1-cycleId].pos;
    if(arena->wall[p1.x][p1.y] != NONE && arena->wall[p2.x][p2.y] != NONE){
        return 0;
    }
    else if(arena->wall[p1.x][p1.y] != NONE){
        return -55000;
    }
    else if(arena->wall[p2.x][p2.y] != NONE){
        return 55000;
    }
    else if(p1.x == p2.x && p1.y == p2.y){
        return 0;
    }
    
    int dis[MAX_WIDTH][MAX_HEIGHT];
    
    int i,j;
    for(i=0;i<MAX_WIDTH;i++){
        for(j = 0;j < MAX_HEIGHT;j++){
            if(arena->wall[i][j] != NONE){
                dis[i][j] = 1;
            }
            else{
                dis[i][j] = -1;
            }
        }
    }
    //countDis...
    int arr[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
    dis[p1.x][p1.y] = 2;
    dis[p2.x][p2.y] = -2;
    struct Node * head = NULL;
    struct Node * tail = NULL;
    enqueue(&head,&tail,p1);
    enqueue(&head,&tail,p2);
    while(head != NULL){
        COORDINATE cur = front(head);
        dequeue(&head,&tail);
        int nextDis = dis[cur.x][cur.y];
        if(nextDis > 0) nextDis+=1;
        else nextDis -= 1;
        COORDINATE next;
        for(i = 0;i < 4;i++){
            next.x = cur.x + arr[i][0];
            next.y = cur.y + arr[i][1];
            if(dis[next.x][next.y] == -1){
                dis[next.x][next.y] = nextDis;
                enqueue(&head,&tail,next);
            }
        }
    }
    
    int diff = 0;
    for(i=0;i<MAX_WIDTH;i++){
        for(j = 0;j < MAX_HEIGHT;j++){
            if(dis[i][j] >= 2){
                diff += 1;
            }
            else if(dis[i][j] <= -2){
                diff -= 1;
            }
        }
    }
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    diff -= dy * dy * 2 + dx * dx / 4 ;
    return diff;
}

int nextMove(MESSAGE* reply,int cycleId,int depth){
    int s_d[2];
    ARENA arena;
    int i,j;
    for(i=0;i<MAX_WIDTH;i++){
        for(j=0;j<MAX_HEIGHT;j++){
            arena.wall[i][j] = reply->arena.wall[i][j];
        }
    }
    for(i=0;i<2;i++){
        arena.cycle[i].pos = reply->arena.cycle[i].pos;
        arena.cycle[i].dir = reply->arena.cycle[i].dir;
    }
    MaxAct(&arena,cycleId,-60000,60000,depth,s_d);
//    fprintf(f,"%d,%d\n",s_d[0],s_d[1]);
    return s_d[1];
}

void Move(ARENA* arena,DIRECTION d,int i){
    COORDINATE cur;
    cur.x = arena->cycle[i].pos.x;
    cur.y = arena->cycle[i].pos.y;
    
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
    arena->wall[arena->cycle[i].pos.x][arena->cycle[i].pos.y] = i + 1;
    arena->cycle[i].pos.x = cur.x;
    arena->cycle[i].pos.y = cur.y;
}

void unMove(ARENA* arena,DIRECTION d,int i){
    COORDINATE cur;
    cur.x = arena->cycle[i].pos.x;
    cur.y = arena->cycle[i].pos.y;
    
    if(d == NORTH){
        cur.y += 1;
    }
    else if(d == SOUTH){
        cur.y -= 1;
    }
    else if(d == WEST){
        cur.x += 1;
    }
    else if(d == EAST){
        cur.x -= 1;
    }
    arena->cycle[i].pos.x = cur.x;
    arena->cycle[i].pos.y = cur.y;
    arena->wall[arena->cycle[i].pos.x][arena->cycle[i].pos.y] = NONE;
}

void MaxAct(ARENA* arena, int cycleId, int alpha, int beta, int depth, int * score_dir){
    if(depth == 0){
        score_dir[0] = score(arena, cycleId);
        score_dir[1] = -1;
        return;
    }
    int value = -60000;
    DIRECTION dir = 0;
    int i;
    for(i = 0;i < 4;i++){
        Move(arena,i,cycleId);
        int min_score_dir[2];
        MinAct(arena,cycleId,alpha,beta,depth-1,min_score_dir);
//        fprintf(f,"%d - %d , %d\n",depth, min_score_dir[0],i);
        unMove(arena,i,cycleId);
        if(min_score_dir[0] == -59000)   min_score_dir[0] = 59000;
        min_score_dir[1] = i;
        if(value < min_score_dir[0]){
            value = min_score_dir[0];
            dir = min_score_dir[1];
        }
        if(value >= beta){
            score_dir[0] = value;
            score_dir[1] = dir;
            return;
        }
        if(alpha < value){
            alpha = value;
        }
    }
    score_dir[0] = value;
    score_dir[1] = dir;
    return;
}

void MinAct(ARENA* arena, int cycleId, int alpha, int beta, int depth, int * score_dir){
    if(depth == 0){
        score_dir[0] = score(arena, cycleId);
        score_dir[1] = -1;
        return;
    }
    int value = 60000;
    DIRECTION dir = 0;
    int i;
    for(i = 0;i < 4;i++){
        Move(arena,i,1-cycleId);
        int max_score_dir[2];
        MaxAct(arena,cycleId,alpha,beta,depth-1,max_score_dir);
        if(max_score_dir[0] == 59000)   max_score_dir[0] = -59000;
//        fprintf(f,"%d - %d , %d\n",depth, max_score_dir[0],i);
        unMove(arena,i,1-cycleId);
        max_score_dir[1] = i;
        if(value > max_score_dir[0]){
            value = max_score_dir[0];
            dir = max_score_dir[1];
        }
        if(value <= alpha){
            score_dir[0] = value;
            score_dir[1] = dir;
            return;
        }
        if(beta > value){
            beta = value;
        }
    }
    score_dir[0] = value;
    score_dir[1] = dir;
    return;
}
