#ifndef MERGESORT_H
#define MERGESORT_H

#include <stdio.h>

typedef enum _bool{FALSE, TRUE}bool;

typedef struct _node
{
  char ** data;
  struct _node* next;
}node;

int getHeader(node*, char*, int*, FILE**);
int checkString(void*, void*);
int checkInteger(void*, void*);
void mergeSort( node ** ,int, int (*)(void*,void*));
node * merge( node *, node*,int, int (*)(void*,void*));
void subdivide( node*, node**, node**);
void printData( node*, int, int);
bool readData ( node*, int, int, FILE**);
void freeNode ( node*);


#endif
