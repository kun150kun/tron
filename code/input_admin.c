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
#include <simpl.h>
#include <ncurses.h>
#include "message.h"

#define N 3
struct Node{
    int data;
    struct Node* next;
};

void enqueue(struct Node** head,struct Node** tail,int x){
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->data = x;
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

int front(struct Node* head,struct Node* tail){
    if(head != NULL)
        return head->data;
    return -1;
}

void reg(char* fromWhom,MESSAGE msg,int * pNum);
void moveBuffer(struct Node** head,struct Node** tail,int * key);
void nextMove(struct Node** head,struct Node** tail,DIRECTION* d,BOOST* b,int humanId);
void bufferReply(char **fromWhomBuffer,MESSAGE* msgBuffer,DIRECTION* d,BOOST* b,int * id,struct Node** head,struct Node** tail);
void readyReply(char ** fromWhomBuffer,MESSAGE* msgBuffer,int* id);


int main(){
    
    if(name_attach("Input_Admin",NULL) == -1){
        printf("can't not attach name in input_admin\n");
        exit(1);
    }
    
    char* fromWhom = NULL;
    MESSAGE msg,reply;
    char ** fromWhomBuffer = (char **)malloc(N * sizeof(char*));
    MESSAGE * msgBuffer = (MESSAGE *)malloc(N * sizeof(MESSAGE));
    
    struct Node * head[2] = {NULL,NULL};
    struct Node * tail[2] = {NULL,NULL};
    int numOfCourier = 0;
    int id[2] = {-1,-1};
    DIRECTION d[2]={EAST,WEST};
    BOOST b[2] = {NO,NO};
    int keydie = 0;
    
    while(1){
        if(Receive(&fromWhom, &msg, sizeof(msg)) == -1){
            printf("can't not receive in input_admin\n");
            exit(1);
        }
        //register immediately.
        if(msg.type == REGISTER_COURIER || msg.type == REGISTER_KEYBOARD){
            reg(fromWhom,msg,&numOfCourier);
        }
        //ready process.
        else if(msg.type == KEYBOARD_READY){
            if(numOfCourier % 5 == 0){
                reply.type = START;
                if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                    printf("can't not reply in input_admin\n");
                    exit(1);
                }
                fromWhomBuffer[2] = NULL;
            }
            else{
                fromWhomBuffer[2] = fromWhom;
                msgBuffer[2].type = msg.type;
            }
            //readyReply(fromWhomBuffer,msgBuffer,id);
        }
        else if(msg.type == COURIER_READY){
            reply.type = REGISTER_HUMAN;
            if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                printf("can't not reply in input_admin\n");
                exit(1);
            }
        }
        else if(msg.type == INIT){
            id[msg.humanId] = msg.humanId;
            //if(fromWhomBuffer[2] != NULL && msgBuffer[2].type == KEYBOARD_READY)
            reply.type = HUMAN_READY;
            reply.humanId =  msg.humanId;
            if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                printf("can't not reply in input_admin\n");
                exit(1);
            }
        }
        else if(msg.type == FAIL){
            numOfCourier -= 1;
            reply.type = OKAY;
            if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                printf("can't not reply in input_admin\n");
                exit(1);
            }
            if (numOfCourier == 0) {
                break;
            }
        }
        //start game
        else if(msg.type == START){
            numOfCourier += 4;
            if (numOfCourier % 5 == 0 && fromWhomBuffer[2] != NULL && msgBuffer[2].type == KEYBOARD_READY) {
                reply.type = START;
                if(Reply(fromWhomBuffer[2], &reply,sizeof(reply)) == -1){
                    printf("can't not reply in input_admin\n");
                    exit(1);
                }
                fromWhomBuffer[2] = NULL;
            }
            fromWhomBuffer[msg.humanId] = fromWhom;
            msgBuffer[msg.humanId].type = msg.type;
        }
        else if(msg.type == UPDATE){
            fromWhomBuffer[msg.humanId] = fromWhom;
            msgBuffer[msg.humanId].type = msg.type;
            //bufferReply(fromWhomBuffer,msgBuffer,d,b,id,head,tail);
        }
        else if(msg.type == KEYBOARD_INPUT){
            if(numOfCourier % 4 == 0){
                keydie = 1;
                reply.type = END;
                if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                    printf("can't not reply in input_admin\n");
                    exit(1);
                }
                break;
            }
            else{
                fromWhomBuffer[2] = fromWhom;
                msgBuffer[2].type = KEYBOARD_INPUT;
                moveBuffer(head,tail,msg.key);
                bufferReply(fromWhomBuffer,msgBuffer,d,b,id,head,tail);
            }
        }
        //end game
        else if(msg.type == END){
            numOfCourier -= 1;
            reply.type = OKAY;
            if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
                printf("can't not reply in input_admin\n");
                exit(1);
            }
            if (fromWhomBuffer[2] != NULL) {
                keydie = 1;
                reply.type = END;
                if(Reply(fromWhomBuffer[2], &reply,sizeof(reply)) == -1){
                    printf("can't not reply in input_admin\n");
                    exit(1);
                }
                fromWhomBuffer[2] = NULL;
            }
            if(keydie == 1 && numOfCourier % 4 == 0){
                break;
            }
        }
    }
    
    free(fromWhomBuffer);
    free(msgBuffer);
    int i;
    for(i = 0;i < 2;i++){
        if(head[i] != NULL){
            dequeue(&head[i],&tail[i]);
        }
    }
    if(name_detach() == -1){
        printf("can't not detach name in input_admin\n");
        exit(1);
    }
    return 0;
}

void reg(char* fromWhom,MESSAGE msg,int * pNum){
    MESSAGE reply;
    if(msg.type == REGISTER_COURIER){
        if((*pNum) % 4 < 2){
            (*pNum) += 1;
            reply.type = INIT;
        }
        else
            reply.type = FAIL;
    }
    else{
        reply.type = INIT;
    }
    if(Reply(fromWhom, &reply,sizeof(reply)) == -1){
        printf("can't not reply in input_admin\n");
        exit(1);
    }
}

void moveBuffer(struct Node** head,struct Node** tail,int * key){
    int i = 0;
    for(;i < MAX_KEYS;i ++){
        if(key[i] == KEY_LEFT){
            enqueue(&head[0], &tail[0], WEST);
        }
        else if(key[i] == KEY_RIGHT){
            enqueue(&head[0], &tail[0], EAST);
        }
        else if(key[i] == KEY_UP){
            enqueue(&head[0], &tail[0], NORTH);
        }
        else if(key[i] == KEY_DOWN){
            enqueue(&head[0], &tail[0], SOUTH);
        }
        else if(key[i] == 'w'){
            enqueue(&head[1], &tail[1], NORTH);
        }
        else if(key[i] == 'a'){
            enqueue(&head[1], &tail[1], WEST);
        }
        else if(key[i] == 's'){
            enqueue(&head[1], &tail[1], SOUTH);
        }
        else if(key[i] == 'd'){
            enqueue(&head[1], &tail[1], EAST);
        }
        else if(key[i] == 'q'){
            enqueue(&head[1], &tail[1], 4);
        }
        else if(key[i] == 'p'){
            enqueue(&head[0], &tail[0], 4);
        }
    }
}

void nextMove(struct Node** head,struct Node** tail,DIRECTION* d,BOOST* b,int humanId){
    if(head[humanId] != NULL){
        if (front(head[humanId],tail[humanId]) == 4) {
            b[humanId] = YES;
        }
        else{
            DIRECTION x = front(head[humanId],tail[humanId]);
            if(x-d[humanId] != 2 && d[humanId] - x != 2)
                d[humanId] = x;
        }
        dequeue(&head[humanId],&tail[humanId]);
    }
}

void bufferReply(char **fromWhomBuffer,MESSAGE* msgBuffer,DIRECTION* d,BOOST* b,int * id,struct Node** head,struct Node** tail){
    int i;
    for(i = 0;i<2;i++){
        if(id[i] != -1 && fromWhomBuffer[i] != NULL && (msgBuffer[i].type == START || msgBuffer[i].type == UPDATE)){
            nextMove(head,tail,d,b,i);
            msgBuffer[i].type = HUMAN_MOVE;
            msgBuffer[i].humanId = i;
            msgBuffer[i].dir = d[i];
            msgBuffer[i].boost = b[i];
            b[i] = NO;
            if(Reply(fromWhomBuffer[i], &msgBuffer[i],sizeof(msgBuffer[i])) == -1){
                printf("can't not reply in input_admin\n");
                exit(1);
            }
            fromWhomBuffer[i] = NULL;
        }
    }
    if(fromWhomBuffer[2] != NULL && msgBuffer[2].type == KEYBOARD_INPUT){
        MESSAGE reply;
        reply.type = OKAY;
        if(Reply(fromWhomBuffer[2], &reply,sizeof(reply)) == -1){
            printf("can't not reply in input_admin\n");
            exit(1);
        }
        fromWhomBuffer[2] = NULL;
    }
}

void readyReply(char ** fromWhomBuffer,MESSAGE* msgBuffer,int* id){
    int i;
    MESSAGE reply;
    reply.type = HUMAN_READY;
    for(i = 0;i < N - 1;i++){
        if (id[i] != -1 && fromWhomBuffer[i] != NULL && msgBuffer[i].type == INIT) {
            reply.humanId = i;
            if(Reply(fromWhomBuffer[i], &reply,sizeof(reply)) == -1){
                printf("can't not reply in input_admin\n");
                exit(1);
            }
            fromWhomBuffer[i] = NULL;
        }
    }
}
