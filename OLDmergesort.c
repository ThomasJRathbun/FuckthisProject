#include "mergesort.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
//Thomas Rathbun
int getHeader(node * head, char * headerTitle, int * numberOfHeaders, FILE** fp)
{
  char * line;
  char * orig = NULL;
  int chosenHeader =0;
  bool found=FALSE;
  fscanf((*fp),"%ms", &line);

  orig = (char*) malloc( sizeof(char) * strlen(line)+1);
  memcpy( orig, line, strlen(line)+1);
    while( line != NULL )
    {
      if(*numberOfHeaders ==0 )
	{
	  line = strtok( line, ",");
	}
      else
	{
	  line = strtok(NULL,",");
	}

      if ( line == NULL)
	{
	  break;
	}

      if ( strcmp(headerTitle,line) == 0)
	{
	  chosenHeader = *numberOfHeaders;
	  found = TRUE;
	}
      (*numberOfHeaders)++;

    }
    head->data = (char**)malloc(sizeof(char*) * *numberOfHeaders);
    int i;
    for (i =0; i < *numberOfHeaders; i++)
      {
	if( i == 0 )
	  orig = strtok(orig, ",");
	else
	  orig = strtok(NULL, ",");

	head->data[i] = (char*)malloc(sizeof(char) * strlen(orig)+1);
	head->data[i] = orig;
      }

  if ( found )
    return chosenHeader;
  else
    return -1;
}

void freeNode( node* head)
{
  node *temp;
  while( head != NULL )
    {
      temp = head;
      head = head->next;
      free(temp);
    }
  
}

int checkInteger( void* a , void* b )
{
  if (strlen(a) < strlen(b))
    {
      return -1;
    }
  else if (strlen(b) < strlen(a))
    {
      return 1;
    }
  
  if ( (unsigned int)atoi( (char*)a) < (unsigned int)atoi( (char*)b))
    {
      return -1;
    }
  else
    {
      return 1;
    }
  
}

int checkString( void* arg1, void* arg2 )
{
  int i = 0;
  char* a = (char*)arg1;
  char* b = (char*)arg2;
  for( i=0; i < strlen(a);i++)
    {
      a[i] = tolower( a[i]);
    }
  for( i=0; i < strlen(b);i++)
    {
      b[i] = tolower(b[i]);
    }
  i = 0;
  int j =0;
  char c1;
  char c2;
  while( i < strlen(a) || j < strlen(b) )
    {
      while(isspace(a[i]) || a[i] == '\"')
	{
	  i++;
	  if( i > strlen(a))
	    break;
	}
      while(isspace(b[j]) || b[j] == '\"')
	{
	  if( j > strlen(b))
	    break;
	  j++;
	}
      c1 = a[i];
      c2 = b[j];
      if(c1 == c2)
	{
	  i++;
	  j++;
	  continue;
	}
      else if( (int)c1 < (int)c2)
	{
	  return -1;
	}
      else
	{
	  return 1;
	}
      i++;
      j++;
    }
  return 0;
}

bool readData( node * head, int _numHeaders, int chosen, FILE** fp )
{
  char* line = NULL;
  size_t size;
  node * newNode = head;
  int c =0;
  bool isNumber = TRUE;
  while( getline(&line, &size,(*fp)) != -1)
    {
      char * s = line;
      bool onlySpaces = FALSE;
      if (line == "")
	{
	  line = NULL;
	  continue;
	}
      if ( newNode->next == NULL && newNode->data == NULL )
	{
	  newNode->data = (char**)malloc(sizeof(char*) * _numHeaders);
	  newNode->next = NULL;
	}
      else
	{
	  while ( newNode->next != NULL )
	    {
	      node * prev = newNode;
	      newNode = newNode->next;
	      prev->next = newNode;
	    
	    }
	  newNode->next = (node*)malloc(sizeof(node));
	  node * prev = newNode;
	  newNode = newNode->next;
	  prev->next = newNode;
	  
	}

      newNode->data = (char**)malloc(sizeof(char*) * _numHeaders);
      newNode->next = NULL;

      int i = 0;
      char *tok = line;
      char *end = line;
      char * specialTok = NULL;
      bool specialFound = FALSE;
      for (i=0; i<_numHeaders; i++)
	{

	  tok = strsep(&end, ",");
	  if(tok != NULL && tok[0] == '"')
	    {
	      char* tmp = (char*)malloc(sizeof(char)*strlen(tok)+1);
	      strcpy(tmp,tok);
	      tok = strsep(&end, "\"");
	      specialTok = (char*)malloc(sizeof(char) * (strlen(tmp) + strlen(tok) + 3));
	      strcpy(specialTok,tmp);
	      strcat(specialTok,",");
	      strcat(specialTok,tok);
	      strcat(specialTok,"\"");
	      specialFound = TRUE;
	      tok = strsep(&end,",");
	    }
	  

	  if(tok == NULL || tok == "")
	    {
	      newNode->data[i] = (char*)malloc(sizeof(char));
	      newNode->data[i] = "\0";
	      continue;
	    }

	  newNode->data[i] = (char*)malloc(sizeof(char) * strlen(line)+1);
	  if ( specialFound == FALSE)
	    {
	      newNode->data[i] = tok;
	    }
	  else
	    {
	      newNode->data[i] = specialTok;
	      specialFound = FALSE;
	    }
	  int l = 0;
	  char* ind = newNode->data[i];
	  if ( i == chosen )
	    {
	      if ( isNumber == TRUE )
		{
		  for(l=0; l < strlen(newNode->data[i]);l++)
		    {
		      if( ((*ind) - '0') <= 9 )
			{
			  continue;
			}
		      else
			isNumber = FALSE;
		    }
		}
	    }
	}
      line = NULL;
      c++;
    }
  return isNumber;
}


void printData( node * head, int _numHeaders, FILE** fp)
{
  node * curr = head;
  int i = 0;
  int l = 0;
  while( curr != NULL)
    {
      
      for (i = 0 ; i< _numHeaders; i++)
	{ 
	  for ( l = 0; l < sizeof(curr->data[i]); l++)
	    {
	      if( curr->data[i][l] == '\n')
		{
		  curr->data[i][l] = '\0';
		}
	    }
	  if ( (_numHeaders - i) == 1)
	    {	     
	      fprintf((*fp),"%s",curr->data[i]);
	    }
	  else
	    {
	      fprintf((*fp),"%s,",curr->data[i]);
	    }
	}
      fprintf((*fp),"\n");
      curr = curr->next;
    }
  return;
}


node* merge(node * leftList, node* rightList,int index, int (*comp)(void*,void*))
{
  if ( leftList == NULL)
    {
      return rightList;
    }
  if ( rightList == NULL)
    {
      return leftList;
    }
  node * result = NULL;

  if( (*comp)(strcat(leftList->data[index],"\0"),strcat(rightList->data[index],"\0")) <= 0)
      {
	result = leftList;
	result->next = merge(leftList->next,rightList,index,comp);
      }
      else
	{
	  result = rightList;
	  result->next = merge(leftList,rightList->next,index,comp);
      	}

      return result;
}

void subDivide( node * head, node** left, node** right )
{

  if ( head == NULL || head->next == NULL)
    {
      *left  = head;
      *right = NULL;
    }
  else
    {
      node * fast = head->next;
      node * slow = head;
      while( fast != NULL)
	{
	  fast = fast->next;
	  if ( fast != NULL )
	    {
	      fast = fast->next;
	      slow = slow->next;
	    }
	}
      *left  = head;
      *right = slow->next;
      slow->next = NULL;
    }
}

void mergeSort( node ** head, int index,  int (*comp)(void*,void*))
{
  node * left    = NULL;
  node * right   = NULL;
  node * result  = *head;
  if ( result == NULL || result->next == NULL)
    {
      return;
    }
    subDivide( result, &left, &right );
    mergeSort(&left, index, comp);
    mergeSort(&right, index, comp);

  *head = merge(left,right,index, comp);
}
