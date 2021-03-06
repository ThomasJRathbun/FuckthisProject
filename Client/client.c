#include <stdio.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <getopt.h>
#include "mergesort.h"

#define MESSAGESIZE 5   //NUMBER OF BYTES DESCRIBING MESSAGE LENGTH
#define BUFFERSIZE 99999 //MESSAGE LENGTH TOTAL

int dirTraversal(char*,char*,char*,int);
int getHeader(node*, char*, int*, FILE**);
//void readData ( node*, int, FILE**);
//int checkString(char*, char*);
//void printData( node*, int);
void *thread_on_CSV(void*);
void *thread_on_Dir(void*);
void readSocket(int,int,void*);

int main(int argc, char * argv[]){

	int PORT , flag_Dir, flag_Col , flag_out , flag_Host, options= 0;
	char * host_name = (char*)malloc(sizeof(char)*1000);
	char * headerTitle = (char*)malloc(sizeof(char)*1000);
	char * searchDir   = (char*)malloc(sizeof(char)*4095);
	char * outputDir   = (char*)malloc(sizeof(char)*4095);
	memset( headerTitle, '\0',(sizeof(char)*1000));
	memset( searchDir, '\0', (sizeof(char)*4095));
	memset( outputDir, '\0', (sizeof(char)*4095));

	//Getting options
	while((options = getopt(argc, argv , "c:h:p:d:o")) != -1){
		switch(options){
			case 'c':
				flag_Col = 1;
				headerTitle = optarg;
				break;
			case 'h':
				host_name = optarg;
				flag_Host = 1;
				break;
			case 'p':
				PORT = atoi(optarg);
				break;
			case 'd':
				searchDir = optarg;
				flag_Dir = 1;
				break;
			case 'o':
				outputDir = optarg;
				flag_out = 1;
				break;
		}

	}
	printf("PORT #:%d\n", PORT );
	// error checking options provided by user
	if(flag_Col == 0){
		printf("-C option was not provided! Please try again\n");
	}
	if(flag_Dir != 1 || searchDir == NULL){
		searchDir = getcwd(NULL, 0);
	}
	if(flag_out != 1 || outputDir == NULL){
		outputDir = 0;
	}
	if(flag_Host != 1){
		printf("-h option was not provided! Please try again\n");
	}
	if(PORT < 2000){
		printf("Invalide PORT number! Please provide a port greater than or equal to 2000!\n");
	}
	printf("YOU SUCK 1\n");
	//traverse directories to find CSV's
	dirTraversal(searchDir, headerTitle, host_name, PORT);
	//Create a socket in order to send the final request for ALL FILES SORTED
	int AFS_socket_FD = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	struct hostent * server;
	char buffer[256];
	if(AFS_socket_FD <0){
		printf("ERROR OPENING SOCKET AFS!\n");
	}
	server = gethostbyname(host_name);
	if(server == NULL){
		printf("ERROR, NO SUCH HOST!\n");
		exit(0);
	}
	bzero((char*)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr , (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(PORT);

	if( connect(AFS_socket_FD,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0){
		printf("Error Connecting \n");
	}

	/*
	int AFS_socket = socket (AF_INET, SOCK_STREAM , 0);
	struct sockaddr_in server_addr;
	struct hostent * server_IPv4;
	server_IPv4 = gethostbyname(host_name);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = server_IPv4->h_name;
	*/
	//int final_con_status = connect(AFS_socket , (struct sockaddr *) &server_addr , sizeof(server_addr));
	//error check the connection
	/*
	if(final_con_status == -1){
		//failed connection
		printf("The connection failed in MAIN to server \n");
	}
	*/
	//REQUEST FINAL ALL FILES SORTED!!!!!!!!!!!!
	char final_req[] = {'-','-','-','-','-'};
	write(AFS_socket_FD, final_req , 5);
	//IMPLEMENT TOMS CRAZY LOOPS
	char num[5];
	char tmp[5];
	snprintf(num,10,"%d",strlen(headerTitle));
	//char* itoa(sizeof(file_Name.columnToSortBy),num, 10);
	int j,i;
	printf("YOU SUCK 2\n");
	//TODO: TOM's IDEA ?
	for( i = 0; i< 5 - strlen(num); i++ ){
		tmp[i] = 0;
	}
	for( j = 0; j < strlen(num) ; j++){
		i++;
		tmp[i] = num[j];
	}
	//Write the column to the server
	int len = strlen(headerTitle);
	int written = 0;

	//READ INTO BUFFER
	char* AFS_size = (char*)malloc(sizeof(char*)*6);
	char* AFS_Buffer = (char*)malloc(sizeof(char*)*99999);

	while(1){
		//written = write(AFS_socket_FD , tmp + written , 5-written);
		memset(AFS_size,0,6);
		memset(AFS_Buffer, 0 , 99999);

		readSocket(AFS_socket_FD,5,AFS_size);
		if(strcmp(AFS_size ,"-----\0")){
			break;
		}
		readSocket(AFS_socket_FD ,atoi(AFS_size) , AFS_Buffer);

	}
	//n = write(AFS_socket_FD , tmp , sizeof(tmp));
	/*
	if(n<0){
		printf("ERROR writing to the socket\n");
	}
	*/
	//n = read( AFS_socket_FD , AFS_buffer, sizeof(AFS_buffer));
	//PRINT OUT !!!!!!!
	printf("%s\n",AFS_Buffer);
	/*
	//now we listen for a server response
	char * buf = (char*)malloc(sizeof(char*)*255);
	read( client_fd , buf , sizeof(buf));
	*/
	close(AFS_socket_FD);
	printf("YOU SUCK 3\n");
	return 0;
} // end of main

int dirTraversal(char* filename, char* headerTitle, char* host_name, int PORT){
	struct stat sb;
	/*
	if(outputDir != NULL){
		if(stat(outputDir, &sb)!= 0 || !S_ISDIR(sb.st_mode)){
			mkdir(outputDir,0700);
		}
	}
	*/
	printf("dirTraversal:: BEFORE OPENDIR\n");
	printf("dirTraversal:: %s\n",filename);
	DIR * base = opendir(filename);
	printf("dirTraversal:: BEFORE READDIR\n");
	//printf("dirTraversal:: %s\n",);
  	struct dirent * entry = readdir(base);
  	char nextDir[4095] = "\0";
  	char*  output = (char*)malloc(sizeof(char)*4095);
  	printf("YOU SUCK 4\n");
  	while( entry != NULL){
  		switch((int)entry->d_type){
  			case DT_DIR:
  			{
  				// directory found
  				printf("YOU SUCK 5\n");
  				//sleep(60);
  				memset( nextDir, '\0', 4095);
  				if(strcmp( entry->d_name, ".\0")  != 0 && strcmp( entry->d_name, "..\0") != 0){
  					printf("dirTraversal:: STRCMP\n");
  					strcat( nextDir, filename);
  					printf("dirTraversal:: %\n", filename);
	      			strcat( nextDir, "/");
	      			printf("dirTraversal:: %s\n",nextDir);
	      			strcat( nextDir, entry->d_name);
	      			printf("dirTraversal:: %s\n",nextDir);
  				}
  				else{
  		  			entry = readdir(base);
  					break;
  				}
  				printf("dirTraversal:: %s\n",entry->d_name);
  				//go to next directory ?
  				//dirTraversal(nextDir, headerTitle, outputDir);
  				pthread_t threadID_Dir;

  				struct dir_Data * nxt_Dir = malloc(sizeof(struct dir_Data));
  				printf("dirTraversal:: NEXTDIR: %s\n",nextDir);
  				//Populate the structure to pass into thread_on_Dir function
  				strcpy(nxt_Dir->nextDirectory, nextDir);
  				strcpy(nxt_Dir->headerTitle, headerTitle);

  				pthread_create(&threadID_Dir,NULL,thread_on_Dir, nxt_Dir);
  				pthread_join(threadID_Dir, NULL);
  				printf("YOU SUCK 6\n");
  				break;
  			}
  			case DT_REG:
  			{
  				//regular file found, error check for csv then send to server for sorting.
				  //node* head = (node*)malloc(sizeof(node));
			      //head->next = NULL;
			      //node* rows = (node*)malloc(sizeof(node));
  				printf("YOU SUCK 7\n");
			      int number_of_headers = 0;
			      int chosenField =-1;
			      char filePath[4095];
			      memset( filePath, '\0', 4095);
			      if(strcmp(nextDir,filename) != 0){
			      		memset( filePath, '\0', 4095);
		  				strcat(filePath,filename);
		  				strcat(filePath,"/");
		  				strcat(filePath,entry->d_name);
			      }
			      strcat( nextDir, filename);
			      strcat( nextDir, "/");
			      strcat( nextDir, entry->d_name);

			      char* extention = (char*)malloc(sizeof(char)*5);
			      extention[4] = '\0';
			      int i = 0;
			      int c = 3;

			      FILE * unSorted = NULL;
			      node* head = (node*)malloc(sizeof(node));

	  				printf("dirTraversal:: FP: %s\n",filePath);

			      for( i = sizeof(filePath); i > 0; i--){
			      		if( filePath[i] == '\0'){
			      			continue;
			      		}else{
			      			extention[c] = filePath[i];
			  				printf("dirTraversal:: %c\n",filePath[i]);
			      			c--;

			      			}
			      		if(c < 0){
			      					break;
			      		}
			      }
			      		if(strcmp(extention,".csv")!= 0){
			  				printf("dirTraversal:: %s\n",entry->d_name);
			  				printf("dirTraversal:: NEXTDIR: %s\n",nextDir);
			  				printf("dirTraversal:: EXT: %s\n",extention);

			      			printf("File is not correct\n");
			      			_exit(-4);
			      		}
			      		unSorted = fopen(filePath, "r");
			      		chosenField = getHeader(head, headerTitle, &number_of_headers,&unSorted);
			      		if(chosenField == -1){
			      			printf("Header DOES NOT EXIST\n");
			      			_exit(-100);
			      		}
			      		if( number_of_headers != 28){
			      			printf(" Incorrect file structure, Amount of headers must be 28\n");
			      			_exit(-2); // file not compatible
			      		}



			      	struct csv_Data *data_to_sort = malloc(sizeof(struct csv_Data));
			      	data_to_sort->csv_FileName = malloc(sizeof(char)*4096);
			      	data_to_sort->columnToSortBy = malloc(sizeof(char)*1000);
			      	data_to_sort->host_name = malloc(sizeof(char)*1000);
			      	data_to_sort->PORT = PORT;

			      	data_to_sort->csv_FileName = filePath;
			      	data_to_sort->columnToSortBy = headerTitle;
			      	data_to_sort->host_name = host_name;

			      	pthread_t threadID_CSV;
  					pthread_create(&threadID_CSV , NULL , thread_on_CSV , data_to_sort);
  					pthread_join(threadID_CSV, NULL);
  					printf("YOU SUCK 7\n");
			      	break;
  			}
  			printf("dirTraversal:: NEW ENTRY\n");
  			
  		}//end of switch
  		entry = readdir(base);
  	}// end of while
  	return 0;

}//end of dirTrav function
void readSocket(int socket, int bytestoread, void* buffer){
	int bytes = 0;
	int result;
	while(bytes < bytestoread){
		result = read(socket, buffer + bytes, bytestoread - bytes);

		if(result < 1){
			printf("did not read\n");
		}
		bytes +=result;
	}
}
int getHeader(node * head, char * headerTitle, int * numberOfHeaders, FILE** fp)
{
  char * line;
  char * orig = NULL;
  int chosenHeader =0;
  bool found=FALSE;
  fscanf((*fp),"%ms", &line);

  orig = (char*) malloc( sizeof(char) * strlen(line)+1);
  memcpy( orig, line, strlen(line)+1);
    while( line != NULL ){
      	if(*numberOfHeaders ==0 ){
	  		line = strtok( line, ",");
		}else{
	  		line = strtok(NULL,",");
		}

      	if(line == NULL){
	  		break;
		}

      	if(strcmp(headerTitle,line) == 0){
	  		chosenHeader = *numberOfHeaders;
	  		found = TRUE;
		}
      	(*numberOfHeaders)++;
    }

    head->data = (char**)malloc(sizeof(char*) * *numberOfHeaders);
    int i;
    for (i =0; i < *numberOfHeaders; i++){
		if( i == 0 )
	  		orig = strtok(orig, ",");
		else
	  		orig = strtok(NULL, ",");

		head->data[i] = (char*)malloc(sizeof(char) * strlen(orig)+1);
		head->data[i] = orig;
    }

  	if(found )
    	return chosenHeader;
  	else
    	return -1;
}

void freeNode( node* head){
  node *temp;
  while( head != NULL ){
      temp = head;
      head = head->next;
      free(temp);
    }

}

int checkInteger( void* a , void* b ){
  if(strlen(a) < strlen(b)){
      return -1;
    }else if (strlen(b) < strlen(a)){
      return 1;
    }

  if((unsigned int)atoi( (char*)a) < (unsigned int)atoi( (char*)b)){
      return -1;
    }else{
      return 1;
    }
}
/*
int checkString( void* arg1, void* arg2 )
{
  int i = 0;
  char* a = (char*)arg1;
  char* b = (char*)arg2;
  for( i=0; i < strlen(a);i++){
      a[i] = tolower( a[i]);
  }
  for( i=0; i < strlen(b);i++){
      b[i] = tolower(b[i]);
  }
  i = 0;
  int j =0;
  char c1;
  char c2;
  while( i < strlen(a) || j < strlen(b) ){
      	while(isspace(a[i]) || a[i] == '\"'){
	  		i++;
	  		if( i > strlen(a))
	    		break;
	  	}
      	while(isspace(b[j]) || b[j] == '\"'){
	  		if( j > strlen(b))
	    		break;
	  		j++;
		}
      	c1 = a[i];
      	c2 = b[j];
      	if(c1 == c2){
	  		i++;
	  		j++;
	  		continue;
		}
      	else if( (int)c1 < (int)c2){
	  		return -1;
		}
      	else{
	  		return 1;
		}
      	i++;
      	j++;
    }
  return 0;
}
*/
/*
bool readData( node * head, int _numHeaders, int chosen, FILE** fp )
{
  char* line = NULL;
  size_t size;
  node * newNode = head;
  int c =0;
  bool isNumber = TRUE;
  	while( getline(&line, &size,(*fp)) != -1){
      char * s = line;
      bool onlySpaces = FALSE;
      	if (line == ""){
	  		line = NULL;
	  		continue;
		}
      	if ( newNode->next == NULL && newNode->data == NULL ){
	  		newNode->data = (char**)malloc(sizeof(char*) * _numHeaders);
	  		newNode->next = NULL;
		}
      	else{
	  		while ( newNode->next != NULL ){
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
*/
/*
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
*/
void *thread_on_CSV(void*data){
	/*
	Client:Spawn a threadd for each csv found
	0. each thread should read in the file
	1. construct a search request
	2. connect to the server
	3. then wait for and read back a response
	4. once the client has sent all files you must send a request FOR ALL files sorted
	5. when the server sends the ALL_FILES_SORTED the client must STDout of the file name
	*/
	printf("YOU SUCK 8\n");
	struct csv_Data * file_Name = data;
	FILE * fp = fopen(file_Name->csv_FileName, "r");
	printf("FILENAME: %s\n", file_Name->csv_FileName);

	if(fp == NULL){
		//file is empty , end thread?
		printf("this file is empty \n");
		pthread_exit(0);
	}

	//connect to the server
	int sockfd;
	int error = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd <0){
		printf("Error opening the socket \n");
	}
	server = gethostbyname(file_Name->host_name);
	if(server == NULL){
		printf("ERROR, INVALID HOST\n");
		exit(0);
	}
	bzero((char*)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(file_Name->PORT);
	printf("PORT:%d\n",file_Name->PORT);
	if( (error = connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) ) < 0){
		printf("ERROR: %s\n",gai_strerror(error));
		printf("Error connecting \n");
	}
	/*
	int client_fd = socket (AF_INET, SOCK_STREAM , 0);
	struct sockaddr_in server_address;

	//Get the IPv4 representation of the host if not supplied from input aka just given a name and not IPv4 dot notation
	struct hostent* server_IPV4_addr;
	server_IPV4_addr = gethostbyname(file_Name->host_name);

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(file_Name->PORT);
	server_address.sin_addr.s_addr = server_IPV4_addr.h_name; //Store the IPv4 address TODO: IF CONNECTION ISSUES CHECK THIS FIRST

	int conn_status = connect(client_fd , (struct sockaddr *) &server_address , sizeof(server_address));

	if(conn_status == -1){
	printf("There was n error when trying to connect to the server !\n");
	}
 	*/
 	//At this point we should be connected to the server
 	/*
 	char* buffer = malloc(sizeOf(char)*255);
 	buffer = file_Name->columnToSortBy;
	write(client_fd, buffer, strlen(buffer));
	*/


	char size[MESSAGESIZE+1]; //to be sent to SERVER  ex: "00011"
	char temp[MESSAGESIZE+1]; //temp hold string (LEN of Message) "11"
	int len = strlen(file_Name->columnToSortBy); //len of message
	memset(size,'0',MESSAGESIZE+1); //set to NULL "00000"
	memset(temp,'0',MESSAGESIZE+1); //set to NULL "00000"
	snprintf(temp,10,"%d", len); //create temporary Length message ex: "11"

	memcpy(size+strlen(temp)+1, temp, strlen(temp)); //create message to be sent "00011"

	int written = 0;
	while(written != MESSAGESIZE)
	{
		written = write(sockfd, size + written , MESSAGESIZE - written );   //write "00011" to server
	}
	written = 0;

	while(written != len)
	{
			written = write(sockfd, file_Name->columnToSortBy + written , len - written );	//write "movie_title" to server (movie_title is 11 bytes
	}

	int title_checker = 1;
	char buffy[BUFFERSIZE];

	while(fgets(buffy, BUFFERSIZE, fp) != NULL)
	{
		printf("IN FILE LOOP!\n");
		// while not at the end of the file send the information.
		//skip the first line of CSV cause its the header information
		if(title_checker == 1){
			title_checker++;
			continue;
		}
			printf("buffy  : %s", buffy);
		len = strlen(buffy); //len of message
		memset(size,'0',MESSAGESIZE+1); //set to NULL "00000"
		memset(temp,'0',MESSAGESIZE+1); //set to NULL "00000"
		snprintf(temp,10,"%d", len); //create temporary Length message ex: "11"

		memcpy(size+(5-strlen(temp)), temp, strlen(temp)); //create message to be sent "00011"

		written = 0;
		while(written != MESSAGESIZE)
		{
			written = write(sockfd, size + written , MESSAGESIZE - written );   //write "00011" to server
		}


			// line is stored in buffy , now we must implement Tom's protocol

			

			written = 0;
			while(written != len){
				written = write(sockfd, buffy + written, len - written);
			}
			memset(buffy , '0', BUFFERSIZE);
		printf("END OF WHILE\n");
	}


	//Write out the signal of EOF
	printf("OUTSIDE WHILE\n");
	char* final_req = "-----\0";
	char* final_num = "00005\0";
//written = 0;
//	while(written != MESSAGESIZE){
//		written = write(sockfd, final_num + written, MESSAGESIZE - written);
//	}
	written = 0;
	while(written != MESSAGESIZE){
		written = write(sockfd, final_req + written, MESSAGESIZE - written);
	}
	//close socket
	printf("YOU SUCK 9\n");
	//sleep(180);
	//close(sockfd);

}

void *thread_on_Dir(void*data){
	printf("YOU SUCK 10\n");
	struct dir_Data * nxtDir = data;
	dirTraversal(nxtDir->nextDirectory , nxtDir->headerTitle , nxtDir->host_name, nxtDir->PORT);
}
