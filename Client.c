#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdarg.h>
#include <dirent.h>
#include <errno.h> 
#include <string.h>
#include <signal.h> 
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BUFSIZE 1024
#define MAX_FILE_SIZE 4096


int  portNumber ,sockfd;
char clientDir[BUFSIZE] ;
char ipAdress[BUFSIZE] ;
char clientName[BUFSIZE] ;
int *socketP;
int status =0 ;


int main(int argc, char** argv) { 

	int i=0,j=0,z=0,y=0,flag=0;
	double workTime ;
	double beforeCalculateArray[16][16] ,afterCalculateArray[16][16];
	double sendArray[256] ;
	double receiveArray[256] ;
	struct timeval  start, end;

	if(argc != 4 ){
        fprintf(stderr,"Wrong line argument, Please enter : portNumber ,IpAddressZ , PoolProSize , PoolForwardSize , Sleeptime\n");
        exit(EXIT_FAILURE);
    }
	
	gettimeofday(&start, NULL);

	strcpy(clientName,argv[1]) ;//strcpy(clientName,argv[1]);
    strcpy(ipAdress,argv[2]);   //strcpy(ipAdress,"192.168.1.26") ;//
	portNumber=atoi(argv[3]);


	sockfd =socket(AF_INET,SOCK_STREAM,0);

    if(sockfd == -1){
        fprintf(stderr,"Socket couldn't be opened for client \n");
        exit (EXIT_FAILURE);           
    }

    struct sockaddr_in addr;
    addr.sin_family =AF_INET;
    addr.sin_port =htons(portNumber);
    addr.sin_addr.s_addr =inet_addr(ipAdress);

    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_in)) == -1){
        fprintf(stderr,"Connection error! Client  couldn't connect to server\n");
        exit (EXIT_FAILURE);
    }

    socketP =malloc(sizeof(int));
    *socketP =sockfd;

    /*------------WriteClientName---------------------*/
    if(write(sockfd,clientName,strlen(clientName)) == -1){
        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
        exit (EXIT_FAILURE);
    }

    /*------------StatuControl---------------------*/
    read(sockfd,&status, sizeof(int));
    if(status==1){

	    /*-----------Create Array-------------------*/
	    srand(time(0));

	    for (i = 0; i < 16; i++){
	        for (j = 0; j < 16; j++){
	        	double temp = rand()%10 ;
	            beforeCalculateArray[i][j]=temp ;
	        }
	    }

	    /*---------------Logger---------------*/
		fprintf(stderr,"--------The matrix that was created--------\n");
		for (i = 0; i < 16; i++){
	        for (j = 0; j < 16; j++){
	            fprintf(stderr,"%.3f ",beforeCalculateArray[i][j]);
	        }
	        fprintf(stderr,"\n");
	    }
		/*------------------------------------*/

	    /*-----------ZIP Array-------------------*/
	    for (i = 0; i < 16; i++){
	        for (j = 0; j < 16; j++){
	            sendArray[z++]=beforeCalculateArray[i][j] ;
	        }
	    }

	    //fprintf(stderr,"sendArray :%f\n",sendArray[5]);

	    if(write(sockfd,sendArray,sizeof(double)*256) == -1){
	        printf("Write Error! Client couldn't write to socket\n");
	        exit (EXIT_FAILURE);
	    }


	    read( sockfd, receiveArray, sizeof(double)*256);
	    //fprintf(stderr,"ServerY den alınan : %f\n",receiveArray[125]);

	    z=0;
	    for (i = 0; i < 16; i++){
	        for (j = 0; j < 16; j++){
	            afterCalculateArray[i][j]=receiveArray[z] ;
	            z++ ;
	        }
	    }

	    /*---------------Logger---------------*/
		fprintf(stderr,"--------The matrix that was calculated--------\n");
		for (i = 0; i < 16; i++){
	        for (j = 0; j < 16; j++){
	            fprintf(stderr,"%.3f ",afterCalculateArray[i][j]);
	        }
	        fprintf(stderr,"\n");
	    }
		/*------------------------------------*/

	    gettimeofday(&end, NULL);
	    workTime=(double) (end.tv_usec - start.tv_usec) / 1000000 +(double) (end.tv_sec - start.tv_sec) ;
	    fprintf(stderr,"Client Work time = %f seconds\n",workTime);

	    /*----------------------WriteToTime---------------------------*/
	    if(write(sockfd,&workTime,sizeof(double)) == -1){
	        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
	        exit (EXIT_FAILURE);
	    }

    }
    else{
    	fprintf(stderr, "NO THREAD IS AVAILABLE\n");
    }
    
    close(sockfd);
    close(*socketP);
    free(socketP);
}






