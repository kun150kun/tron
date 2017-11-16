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
#include <simpl.h>
#include "message.h"
#include <stdlib.h>

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

void bufferReply(char** bufferFromWhom,struct Node ** head,struct Node ** tail, int winId);

int main(){
    if(name_attach("Display_Admin",NULL) == -1){
        printf("can't not attach name in display admin\n");
        exit(1);
    }
    char* fromWhom = NULL;
    MESSAGE msg,reply;
    char* bufferFromWhom = NULL;
    
    struct Node * head = NULL;
    struct Node * tail = NULL;
    int winId = -2;
    while(1){
        if(Receive(&fromWhom, &msg, sizeof(msg)) == -1){
            printf("can't not receive in display admin\n");
            exit(1);
        }
        if(msg.type == DISPLAY_ARENA){
            reply.type = OKAY;
            if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
                printf("can't not reply in display admin\n");
                exit(1);
            }
            
            enqueue(&head,&tail,msg.arena);
            if(bufferFromWhom!=NULL){
                bufferReply(&bufferFromWhom,&head,&tail,-2);
            }
        }
        else if(msg.type == PAINTER_READY){
            if(head != NULL){
                if(winId != -2 && head == tail){
                    reply.type = END;
                    reply.cycleId = winId;
                }
                else{
                    reply.type = PAINT;
                }
                reply.arena = front(head,tail);
                if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
                    printf("can't not reply in display admin\n");
                    exit(1);
                }
                dequeue(&head,&tail);
                if(winId != -2 && head == NULL) break;
            }
            else{
                bufferFromWhom = fromWhom;
            }
        }
        else if(msg.type == END){
            reply.type = OKAY;
            if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
                exit(1);
            }
            winId = msg.cycleId;
            enqueue(&head,&tail,msg.arena);
            winId = msg.cycleId;
            if(bufferFromWhom != NULL){
                bufferReply(&bufferFromWhom,&head,&tail, winId);
            }
            if(head == NULL){
                break;
            }
        }
    }
    if(name_detach() == -1){
        printf("can't not detach name in display admin\n");
        exit(1);
    }
    return 0;
}

void bufferReply(char** bufferFromWhom,struct Node ** head,struct Node ** tail, int winId){
    if(*head != NULL){
        MESSAGE reply;
        reply.type = PAINT;
        if(winId != -2 && *head == *tail){
            reply.type = END;
            reply.cycleId = winId;
        }
        reply.arena = front(*head,*tail);
        if(Reply(*bufferFromWhom, &reply, sizeof(reply)) == -1){
            exit(1);
        }
        *bufferFromWhom = NULL;
        dequeue(head,tail);
    }
}
