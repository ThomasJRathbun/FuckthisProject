/*
 * sortedmerge.c
 *
 *  Created on: Dec 12, 2017
 *      Author: tjr138
 */


#include "sortedmerge.h"

node* sortedMerge( node* a, node* b, int col, int (*comp)(void*,void*))
{
  node* Source = NULL;
  node* head = NULL;

  //if one of the lists is empty return the other
  if( b == NULL)
    return a;
  else if ( a == NULL )
    return b;

  //set source and head to the first node
 if ( comp(a->data[col], b->data[col]) <= 0 )
    {
      Source = a;
      a = Source->next;
    }
  else
    {
      Source = b;
      b = Source->next;
    }
  head = Source;

  //set the source to the first node
  while( a != NULL && b != NULL)
    {
      if ( comp(a->data[col], b->data[col]) <= 0 )
	{
	  Source->next = a;
	  Source = a;
	  a = Source->next;
	}
      else
	{
	  Source->next = b;
	  Source = b;
	  b = Source->next;
	}
    }
  //if a or b is NULL, append the last node to the list
  //and return the head node
  if ( a == NULL )
    {
      Source->next = b;
    }
  else if ( b == NULL)
    {
      Source->next = a;
    }

  return head;


}
