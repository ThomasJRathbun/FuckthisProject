#ifndef MERGESORT_H
#define MERGESORT_H

#include  <unistd.h>
#include  <stdlib.h>
#include  <stdio.h>

typedef enum _bool{FALSE, TRUE}bool;

typedef struct _node
{
  char ** data;
  struct _node* next;
}node;

struct dir_Data{
	char* nextDirectory;//4095
	char* headerTitle;//1000
	char* host_name;//[1000];
	
	int PORT;
};

struct csv_Data{
	char* csv_FileName ; //4096
	char* host_name; //1000
	int PORT;
	char* columnToSortBy; //1000
	
};

int checkString(char*, char*);
void mergeSort( node ** ,int, int (*)(void*,void*));
node * merge( node *, node*,int, int (*)(void*,void*));
void subdivide( node*, node**, node**);
void printData( node*, int);
//void readData ( node*, int);
void freeLinkedList(node*);
int getHeader(node*, char*, int*, FILE**);

#endif
