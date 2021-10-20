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
#include <arpa/inet.h>
#include<math.h>

#define PI 3.14159265
#define BUFSIZE 1024
#define MAX_FILE_SIZE 4096

pthread_t* PoolProThreads ;
pthread_t* PoolForwarThreads ;
pthread_mutex_t lock, lock2;

int portNumberC,PoolProSize,PoolForwardSize,Sleeptime=0 ;
double totalTime=0.0 ;
int totalPro=0 ,totalForward=0 ;

volatile sig_atomic_t isServerOpen = 1;

char IpAddressZ[20] ;
int portNumbeZ ;
int threadNum=0 ;
int ForwardThreadNum=0 ;
int sockfd;
FILE *logfp;
char logStr[BUFSIZE];

void signalhandler(int sig) ;
double* FourierTransform(double inputData[16][16]) ;

void *conThreadFunc(void *vargp) { 

	int *myid = (int *)vargp;
	char clientName[BUFSIZE];
	double beforeCalculateArray[16][16] ,afterCalculateArray[16][16],receiveArray[256];
	double *sendArray;
	double workTime=0.0;
	int i=0,j=0,z=0,status=0 ;

	/*------------ClientName---------------------*/

	read( *myid , clientName, BUFSIZE);
    fprintf(stderr,"Client Name: %s\n",clientName);
    /*---------------Logger---------------*/
	logfp=fopen(logStr,"a+");
	fprintf(logfp,"Name of the client that connected: %s\n",clientName);
	fclose(logfp);
	/*------------------------------------*/

	/*------------StatuControl---------------------*/
	status=1 ;
	if(write(*myid,&status,sizeof(int)) == -1){
        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
        exit (EXIT_FAILURE);
    }
    /*---------------------------------------------*/

	read( *myid , receiveArray, sizeof(double)*256);
    //fprintf(stderr,"Client dan gelen : %f\n",receiveArray[5]);

    for (i = 0,z=0; i < 16; i++){
        for (j = 0; j < 16; j++){
            beforeCalculateArray[i][j]=receiveArray[z] ;
            z++ ;
        }
    }

    sendArray=FourierTransform(beforeCalculateArray) ;
    //fprintf(stderr,"Client a gidecek : %f\n",sendArray[125]);

    for (i = 0,z=0; i < 16; i++){
        for (j = 0; j < 16; j++){
            afterCalculateArray[i][j]=sendArray[z] ;
            z++ ;
        }
    }

	/*----------------SLEEP--------------------*/
    sleep(Sleeptime);
    /*-----------------------------------------*/

	if(write(*myid,sendArray,sizeof(double)*256) == -1){
        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
        exit (EXIT_FAILURE);
    }

    /*----------------------ReadToTime---------------------------*/
    if(read(*myid,&workTime,sizeof(double)) == -1){
        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
        exit (EXIT_FAILURE);
    }

    //fprintf(stderr,"Total time = %f seconds\n",workTime);

    close(*myid);
    free(myid);
    free(sendArray);
    
    /*---------------------------LOCK------------------------*/
    pthread_mutex_lock(&lock);

    --threadNum;
    totalPro ++ ;
    totalTime=totalTime+workTime ;

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
	/*------------------------------------*/
	/*---------------Logger---------------*/
	fprintf(logfp,"It was handled by PoolPro\n");
	fprintf(stderr,"It was handled by PoolPro\n");
	fprintf(logfp,"Total time of calculation :%f\n",workTime);
	fprintf(stderr,"Total time of calculation :%f\n",workTime);
	fclose(logfp);
	/*------------------------------------*/

    pthread_mutex_unlock(&lock);
    /*---------------------------UNLOCK------------------------*/
}

void *ForwarrdThreadFunc(void *vargp) {

	int *myid = (int *)vargp;
	int *socketC;
	char clientName[BUFSIZE];
	double beforeCalculateArray[16][16] ,afterCalculateArray[16][16] ;
	double receiveArray[256],receiveArrayFromZ[256];
	double workTime=0.0;
	int i=0,j=0,z=0,status=1,statusZ=0,control=0;

	/*------------ClientName---------------------*/

	read( *myid , clientName, BUFSIZE);
    fprintf(stderr,"Client Name: %s\n",clientName);

    /*---------------Logger---------------*/
	logfp=fopen(logStr,"a+");
	fprintf(logfp,"Name of the client that connected: %s\n",clientName);
	fclose(logfp);
	/*------------------------------------*/

	/*----------ServerZ geçiş yapılıyor------------*/

	sockfd =socket(AF_INET,SOCK_STREAM,0);

    if(sockfd == -1){
        fprintf(stderr,"Socket couldn't be opened for ServerZ \n");
        exit (EXIT_FAILURE);           
    }

    struct sockaddr_in addr;
    addr.sin_family =AF_INET;
    addr.sin_port =htons(portNumbeZ);
    addr.sin_addr.s_addr =inet_addr(IpAddressZ);

    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_in)) == -1){
        fprintf(stderr,"Connection error! Client  couldn't connect to serverZ\n");
        status=0 ;
        control=1 ;
		if(write(*myid,&status,sizeof(int)) == -1){
	        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
	        exit (EXIT_FAILURE);
	    }
        //exit (EXIT_FAILURE);
    }

    if(control==0){

	    socketC =malloc(sizeof(int));
	    *socketC =sockfd;

	    //fprintf(stderr,"ServerZ ye gidecek : %f\n", receiveArray[5]);

	    /*------------StatuControlForServerZ---------------------*/
		read(sockfd,&statusZ, sizeof(int));
		//fprintf(stderr,"ServerZ status : %d\n", statusZ);
	    /*-------------------------------------------------------*/
		if(statusZ==1){

			/*------------StatuSendingForClient---------------------*/
			status=1 ;
			if(write(*myid,&status,sizeof(int)) == -1){
		        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
		        exit (EXIT_FAILURE);
		    }
		    /*---------------------------------------------*/

			read( *myid , receiveArray, sizeof(double)*256);
		    //fprintf(stderr,"Client dan gelen : %f\n",receiveArray[5]);

		    for (i = 0; i < 16; i++){
		        for (j = 0; j < 16; j++){
		            beforeCalculateArray[i][j]=receiveArray[z] ;
		            z++ ;
		        }
		    }

			if(write(*socketC,receiveArray,sizeof(double)*256) == -1){
		        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
		        exit (EXIT_FAILURE);
		    }

		    read( *socketC , receiveArrayFromZ, sizeof(double)*256);
		    //fprintf(stderr,"ServerZ den gelen : %f\n",receiveArrayFromZ[5]);
		    z=0 ;
		    for (i = 0; i < 16; i++){
		        for (j = 0; j < 16; j++){
		            afterCalculateArray[i][j]=receiveArray[z] ;
		            z++ ;
		        }
		    }

		    //fprintf(stderr,"Client a gidecek : %f\n",receiveArrayFromZ[5]);
		    if(write(*myid,receiveArrayFromZ,sizeof(double)*256) == -1){
		        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
		        exit (EXIT_FAILURE);
		    }

		    /*----------------------ReadToTime---------------------------*/
		    if(read(*myid,&workTime,sizeof(double)) == -1){
		        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
		        exit (EXIT_FAILURE);
		    }
		    //fprintf(stderr,"Total time = %f seconds\n",workTime);		    
		}
		else{
			/*------------StatuSendingForClient---------------------*/
			status=0 ;
			if(write(*myid,&status,sizeof(int)) == -1){
		        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
		        exit (EXIT_FAILURE);
		    }
		    /*---------------------------------------------*/
		}	      
	}
	else{
		/*---------------Logger---------------*/
		logfp=fopen(logStr,"a+");
		fprintf(logfp,"NO THREAD IS AVAILABLE\n");
		fclose(logfp);
		/*------------------------------------*/
	}


	if(control==0){
	    close(*socketC);
	    free(socketC);
	}

	/*---------------------------LOCK------------------------*/
    pthread_mutex_lock(&lock2);
    --ForwardThreadNum;
    totalForward++ ;
    totalTime=totalTime+workTime ;

    if(statusZ==1){

    	/*---------------Logger---------------*/
		logfp=fopen(logStr,"a+");
		fprintf(logfp,"-------The matrix forward to ServerZ--------\n");
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
		/*------------------------------------*/
		/*---------------Logger---------------*/
		fprintf(logfp,"It was handled by PoolForward\n");
		fprintf(stderr,"It was handled by PoolForward\n");
		fprintf(logfp,"Total time of calculation :%f\n",workTime);
		fprintf(stderr,"Total time of calculation :%f\n",workTime);
		fclose(logfp);
		/*------------------------------------*/

    }
    pthread_mutex_unlock(&lock2);
    /*---------------------------UNLOCK------------------------*/
}


int main(int argc, char** argv) { 

	int portNumberC,PoolProSize,PoolForwardSize ;
	int i,j, fdServer, fdSocket,opt=1,status=0 ,control=0;
	int *socketP;
	char clientName[BUFSIZE];

    struct sockaddr_in addr;
    int addrlen=sizeof(struct sockaddr_in);

    if(argc != 8 ){
        fprintf(stderr,"Wrong line argument, Please enter : portNumberC ,IpAddressZ , PoolProSize , PoolForwardSize , Sleeptime\n");
        exit(EXIT_FAILURE);
    }

	portNumberC=atoi(argv[1]);
	strcpy(IpAddressZ,argv[2]);  //strcpy(IpAddressZ,"192.168.1.26");
	portNumbeZ=atoi(argv[3]);
	PoolProSize=atoi(argv[4]);
    PoolForwardSize=atoi(argv[5]);
    Sleeptime=atoi(argv[6]);
    strcpy(logStr,argv[7]);

    /*---------------Logger---------------*/
	logfp=fopen(logStr,"w");
	fprintf(logfp,"--------START SERVERY----------\n");
	fclose(logfp);
	/*------------------------------------*/

    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&lock2, NULL) != 0){
        printf("\n mutex init failed\n");
        return 1;
    }

	PoolProThreads=malloc(sizeof(pthread_t)*(PoolProSize));

	PoolForwarThreads=malloc(sizeof(pthread_t)*(PoolForwardSize));

	fdServer =socket(AF_INET,SOCK_STREAM,0);
    if(fdServer == -1){
        perror("Socket couldn't be opened \n");
        exit (EXIT_FAILURE);           
    }


    if (setsockopt(fdServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }

    memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;  
    addr.sin_port = htons(portNumberC);
    addr.sin_addr.s_addr =htonl(INADDR_ANY); 


    if(bind(fdServer,(struct sockaddr*)&addr,sizeof(struct sockaddr_in)) == -1){
        perror("Server couldn't bind socket\n");
        exit (EXIT_FAILURE);       
    }
    if(listen(fdServer,PoolProSize) == -1){
        perror("Server couldn't listen socket\n");
        exit (EXIT_FAILURE);
    }
    fprintf(stderr,"ServerY is waiting for client connections at port %d\n",portNumberC);

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

	    	if(ForwardThreadNum<PoolForwardSize){

	            if(threadNum<PoolProSize){

			        if(pthread_create(&PoolProThreads[threadNum],NULL,conThreadFunc,socketP) !=0){
			            fprintf(stderr,"Thread couldn't be created \n");
			            exit (EXIT_FAILURE);
			        }
			        ++threadNum;
			        control=1 ;			        
			    }
			    else{
			    	/*-------------Thread Sınırına ulaştı ve ServerZ geçiş yapılıyor --------------------------*/			    	
			    	if(pthread_create(&PoolForwarThreads[ForwardThreadNum],NULL,ForwarrdThreadFunc,socketP) !=0){
			            fprintf(stderr,"Thread couldn't be created \n");
			            exit (EXIT_FAILURE);
			        }

			        ++ForwardThreadNum;
			    }		    
			}
			else{				
				fprintf(stderr, "NO THREAD IS AVAILABLE\n");

				read(fdSocket, clientName, BUFSIZE);
    			fprintf(stderr,"Connected : %s\n",clientName);
    			/*---------------Logger---------------*/
    			logfp=fopen(logStr,"a+");
    			fprintf(logfp,"Name of the client that connected: %s\n",clientName);
    			fprintf(logfp,"NO THREAD IS AVAILABLE\n");
    			fclose(logfp);
    			/*------------------------------------*/
				status=0 ;
				if(write(fdSocket,&status,sizeof(int)) == -1){
			        fprintf(stderr,"Write Error! Client couldn't write to socket\n");
			        exit (EXIT_FAILURE);
			    }	
			}
		}
	}

	if(control==1){
	    for(i=0;i<PoolProSize;++i)
	        pthread_join(PoolProThreads[i], NULL);

	    for(i=0;i<PoolForwardSize;++i)
	        pthread_join(PoolForwarThreads[i], NULL);
	}

	free(PoolProThreads);
	free(PoolForwarThreads);

	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&lock2);

	close(fdServer);

    return (EXIT_SUCCESS);
}


double* FourierTransform(double inputData[16][16]){

	double realOut[16][16], imagOut[16][16] ,amplitudeOut[16][16] ;
   	int height = 16, width = 16 ,yWave,xWave,ySpace,xSpace;
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

	fprintf(logfp,"Total number of client connections : %d\n",totalPro+totalForward);

	fprintf(logfp,"Ratio of connections handled by PoolPro : %d\n",totalPro);

	fprintf(logfp,"Ratio of connections handled by PoolForward : %d\n",totalForward);

	fprintf(logfp,"Mean processing time of every request : %f\n",totalTime/(totalPro+totalForward));

	fprintf(logfp,"SERVER SHUTDOWN\n");

	fclose(logfp);
	/*-----------------------*/

    isServerOpen =0;
}