//[include할 다른 헤더 파일 명시]
#ifndef STDLIB_H
# define STDLIB_H
#endif
#include <stdlib.h>

//매크로 선언
#define MAX_INPUT 1024
#define MAX_HEAP 500000
//구조체, type 선언

typedef enum { false, true } bool;

typedef struct _Node {
    int item;
    struct _Node* next;
} Node;

typedef struct {
    Node *head;
    int len;
} LinkedList;

void InitList(LinkedList *plist) {
    plist->head = (Node *) malloc(sizeof(Node));
    plist->head->next = NULL;
    plist->len = 0;
}

bool IsEmpty(LinkedList *plist) {
    return plist->len == 0;
}

void Insert(LinkedList *plist, int pos, int item) {
    Node *cur, *newNode;
    if (pos < 0 || pos > plist->len) exit(1);

    newNode = (Node *)malloc(sizeof(Node));
    newNode->item = item;
    newNode->next = NULL;

    cur = plist->head;
    for (int i = 0; i < pos; i++) cur = cur->next;

    newNode->next = cur->next;
    cur->next = newNode;
    plist->len++;
}

void RemoveLast(LinkedList *plist) {
    Node *cur, *temp;
    if (IsEmpty(plist)) return;

    cur = plist->head;
    while (cur->next != NULL) {
        cur = cur->next;
    }

    plist->len--;
    free(cur);
}

void ClearList(LinkedList *plist) {
    while (plist->head->next != NULL)
        RemoveLast(plist);
    free(plist->head);
}

//[전역 변수 선언]

//[함수 선언]
int max(int a, int b)
{
    if(a > b)
        return a;
    else
        return b;
}

int length(const char* str)
{
    int cnt;
    for (cnt = 0; str[cnt] != '\0'; cnt++);
    return cnt;
}