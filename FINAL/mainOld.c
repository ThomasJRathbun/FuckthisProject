#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include "mergesort.h"
#include "Server.h"
#include "sortedmerge.h"
#include <sys/stat.h>


int main( int argc, char ** argv )
{

	int options;
	int flagP =0;
	char* port = (char*)malloc(sizeof(char)*6);

	while( (options = getopt(argc,argv,"p:")) != -1 )
	{
		switch(options)
		{
		case 'p':
			port = optarg;
			flagP = 1;
			break;

		}
	}
	if( flagP == 0 )
	{
		printf( "No -p option: Please try again with a port\n");
		return -1;
	}
	//printf("%s\n",port);
	acceptConnections(port);

  return 0;
}
