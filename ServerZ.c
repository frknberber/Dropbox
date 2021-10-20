#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
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

#define BUFSIZE 1024
#define MAX_FILE_SIZE 4096

pthread_t* PoolProThreads ;
pthread_mutex_t lock;

volatile sig_atomic_t isServerOpen = 1;

char ipAdress[20] ;
int threadNum=0;
FILE *logfp;
char logStr[BUFSIZE];
int Sleeptime=0 ;

void signalhandler(int sig) ;
double* FourierTransform(double inputData[16][16]) ;

void *conThreadFunc(void *vargp) { 

	int *myid = (int *)vargp;
	double receiveArray[256] ;
	double beforeCalculateArray[16][16] ,afterCalculateArray[16][16] ;
	double *sendArray;
	int i,j,z ;
	int status =0;

	
	/*----Sent for avaliable status------*/
	status=1 ;
	if(write(*myid,&status,sizeof(int)) == -1){
        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
        exit (EXIT_FAILURE);
    }
    /*-----------------------------------*/

	read( *myid, receiveArray, sizeof(double)*256);
    //fprintf(stderr,"ServerY den alınan : %f\n",receiveArray[5]);

    for (i = 0; i < 16; i++){
        for (j = 0; j < 16; j++){
            beforeCalculateArray[i][j]=receiveArray[z] ;
            z++ ;
        }
    }

    sendArray=FourierTransform(beforeCalculateArray) ;
    //fprintf(stderr,"ServerY e gidecek : %f\n",sendArray[5]);

    for (i = 0,z=0; i < 16; i++){
        for (j = 0; j < 16; j++){
            afterCalculateArray[i][j]=sendArray[z] ;
            z++ ;
        }
    }

    sleep(Sleeptime);

	if(write(*myid,sendArray,sizeof(double)*256) == -1){
        fprintf(stderr,"Write Error! ServerZ couldn't write to socket\n");
        exit (EXIT_FAILURE);
    }

    close(*myid);
    free(myid);
    free(sendArray);

    /*---------------------------LOCK------------------------*/
    pthread_mutex_lock(&lock);
    --threadNum;

    /*---------------Logger---------------*/
	logfp=fopen(logStr,"a+");
	fprintf(logfp,"-------The matrix that was processed--------\n");
	for (i = 0; i < 16; i++){
        for (j = 0; j < 16; j++){
            fprintf(logfp,"%.3f ",beforeCalculateArray[i][j]);
        }
        fprintf(logfp,"\n");
    }
	/*------------------------------------*/
	/*---------------Logger---------------*/
	fprintf(logfp,"-------The output of calculations--------\n");
	for (i = 0; i < 16; i++){
        for (j = 0; j < 16; j++){
            fprintf(logfp,"%.3f ",afterCalculateArray[i][j]);
        }
        fprintf(logfp,"\n");
    }
	fclose(logfp);
	/*------------------------------------*/

    pthread_mutex_unlock(&lock);
    /*---------------------------UNLOCK------------------------*/
}


int main(int argc, char** argv) { 

	int portNumberC,IpAddressZ,PoolProSize,PoolForwardSize ;
	int i,j, fdServer, fdSocket,opt=1 ,control=0;
	int *socketP;
	int status =0;
	
    struct sockaddr_in addr;
    int addrlen=sizeof(struct sockaddr_in);  

    if(argc != 5 ){
        fprintf(stderr,"Wrong line argument, Please enter : portNumberC ,IpAddressZ , PoolProSize , PoolForwardSize , Sleeptime\n");
        exit(EXIT_FAILURE);
    }

	portNumberC=atoi(argv[1]);
	PoolProSize=atoi(argv[2]);
    Sleeptime=atoi(argv[3]);
    strcpy(logStr,argv[4]);

    /*---------Logger--------*/
	logfp=fopen(logStr,"w");
	fprintf(logfp,"--------START SERVERZ----------\n");
	fclose(logfp);
	/*-----------------------*/

    if (pthread_mutex_init(&lock, NULL) != 0){
        fprintf(stderr,"\n mutex init failed\n");
        return 1;
    }
	
	PoolProThreads=malloc(sizeof(pthread_t)*(PoolProSize));

	fdServer =socket(AF_INET,SOCK_STREAM,0);
    if(fdServer == -1){
        fprintf(stderr,"Socket couldn't be opened \n");
        exit (EXIT_FAILURE);           
    }


    if (setsockopt(fdServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        fprintf(stderr,"setsockopt"); 
        exit(EXIT_FAILURE); 
    }

    memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;  
    addr.sin_port = htons(portNumberC);
    addr.sin_addr.s_addr =htonl(INADDR_ANY); 


    if(bind(fdServer,(struct sockaddr*)&addr,sizeof(struct sockaddr_in)) == -1){
        fprintf(stderr,"Server couldn't bind socket\n");
        exit (EXIT_FAILURE);       
    }
    if(listen(fdServer,PoolProSize) == -1){
        fprintf(stderr,"Server couldn't listen socket\n");
        exit (EXIT_FAILURE);
    }

    fprintf(stderr,"ServerZ is waiting for ServerY connections at port %d\n",portNumberC);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    struct sigaction sa;
    memset(&sa,0,sizeof(struct sigaction));
    sa.sa_handler =signalhandler;
    sigaction(SIGINT,&sa,NULL);

    while(isServerOpen){

    	fdSocket =accept(fdServer, (struct sockaddr *)&addr,(socklen_t*)&addrlen);
    	if(fdSocket != -1){
	        
	        socketP =malloc(sizeof(int));
	        *socketP =fdSocket; 

	    	if(threadNum<PoolProSize){

		        if(pthread_create(&PoolProThreads[threadNum],NULL,conThreadFunc,socketP) !=0){
		            fprintf(stderr,"Thread couldn't be created \n");
		            exit (EXIT_FAILURE);
		        }

		        ++threadNum;
		        control=1 ;
			    
			}
			else{
				
				//fprintf(stderr, "NO THREAD IS AVAILABLE IN SERVERZ\n");
				/*---------Logger--------*/
				logfp=fopen(logStr,"a+");
				fprintf(logfp,"NO THREAD IS AVAILABLE IN SERVERZ\n");
				fclose(logfp);
				/*-----------------------*/

				status=0 ;
				if(write(fdSocket,&status,sizeof(int)) == -1){
			        fprintf(stderr,"Write Error! ServerZ couldn't write to socket\n");
			        exit (EXIT_FAILURE);
			    }					
			}
		}
	}


	if(control==1){
	    for(i=0;i<PoolProSize;++i)
	        pthread_join(PoolProThreads[i], NULL);
	}


	free(PoolProThreads);
	pthread_mutex_destroy(&lock);
    close(fdServer);

    return (EXIT_SUCCESS);
}

double* FourierTransform(double inputData[16][16]){

   	int height = 16, width = 16 ;
    int i, j , z=0;

    double *resultArray ;

	resultArray=malloc(sizeof(double)*(256));

    for (i = 0; i < 16; i++){
        for (j = 0; j < 16; j++){
        	double temp = inputData[i][j]*1.2 ;
            resultArray[z++]=temp ;
        }
    }

    return resultArray ;
}

void signalhandler(int sig){
    
    fprintf(stderr, "SERVER SHUTDOWN\n");
	/*---------Logger--------*/
	logfp=fopen(logStr,"a+");
	fprintf(logfp,"SERVER SHUTDOWN\n");
	fclose(logfp);
	/*-----------------------*/
    isServerOpen =0;
}