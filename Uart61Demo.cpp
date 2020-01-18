#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include<fcntl.h>

#include<assert.h>
#include<termios.h>
#include<string.h>
#include<sys/time.h>
#include<time.h>
#include<sys/types.h>
#include<errno.h>
#include <cmath>
#include <vector>
#include <fstream>

static int ret;
static int fd;

#define BAUD 115200 //115200 for JY61 ,9600 for others

// udp part
int sock, n;
   unsigned int length;
   struct sockaddr_in server, from;
   struct hostent *hp;
char udp_buffer[sizeof(double)*7];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
int uart_open(int fd,const char *pathname)
{
    fd = open(pathname, O_RDWR|O_NOCTTY); 
    if (-1 == fd)
    { 
        perror("Can't Open Serial Port"); 
		return(-1); 
	} 
    else
		printf("open %s success!\n",pathname);
    if(isatty(STDIN_FILENO)==0) 
		printf("standard input is not a terminal device\n"); 
    else 
		printf("isatty success!\n"); 
    return fd; 
}

int uart_set(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
     struct termios newtio,oldtio; 
     if  ( tcgetattr( fd,&oldtio)  !=  0) {  
      perror("SetupSerial 1");
	  printf("tcgetattr( fd,&oldtio) -> %d\n",tcgetattr( fd,&oldtio)); 
      return -1; 
     } 
     bzero( &newtio, sizeof( newtio ) ); 
     newtio.c_cflag  |=  CLOCAL | CREAD;  
     newtio.c_cflag &= ~CSIZE;  
     switch( nBits ) 
     { 
     case 7: 
      newtio.c_cflag |= CS7; 
      break; 
     case 8: 
      newtio.c_cflag |= CS8; 
      break; 
     } 
     switch( nEvent ) 
     { 
     case 'o':
     case 'O': 
      newtio.c_cflag |= PARENB; 
      newtio.c_cflag |= PARODD; 
      newtio.c_iflag |= (INPCK | ISTRIP); 
      break; 
     case 'e':
     case 'E': 
      newtio.c_iflag |= (INPCK | ISTRIP); 
      newtio.c_cflag |= PARENB; 
      newtio.c_cflag &= ~PARODD; 
      break;
     case 'n':
     case 'N': 
      newtio.c_cflag &= ~PARENB; 
      break;
     default:
      break;
     } 

     /*设置波特率*/ 

switch( nSpeed ) 
     { 
     case 2400: 
      cfsetispeed(&newtio, B2400); 
      cfsetospeed(&newtio, B2400); 
      break; 
     case 4800: 
      cfsetispeed(&newtio, B4800); 
      cfsetospeed(&newtio, B4800); 
      break; 
     case 9600: 
      cfsetispeed(&newtio, B9600); 
      cfsetospeed(&newtio, B9600); 
      break; 
     case 115200: 
      cfsetispeed(&newtio, B115200); 
      cfsetospeed(&newtio, B115200); 
      break; 
     case 460800: 
      cfsetispeed(&newtio, B460800); 
      cfsetospeed(&newtio, B460800); 
      break; 
     default: 
      cfsetispeed(&newtio, B9600); 
      cfsetospeed(&newtio, B9600); 
     break; 
     } 
     if( nStop == 1 ) 
      newtio.c_cflag &=  ~CSTOPB; 
     else if ( nStop == 2 ) 
      newtio.c_cflag |=  CSTOPB; 
     newtio.c_cc[VTIME]  = 0; 
     newtio.c_cc[VMIN] = 0; 
     tcflush(fd,TCIFLUSH); 

if((tcsetattr(fd,TCSANOW,&newtio))!=0) 
     { 
      perror("com set error"); 
      return -1; 
     } 
     printf("set done!\n"); 
     return 0; 
}

int uart_close(int fd)
{
    assert(fd);
    close(fd);

    return 0;
}
int send_data(int  fd, char *send_buffer,int length)
{
	length=write(fd,send_buffer,length*sizeof(unsigned char));
	return length;
}
int recv_data(int fd, char* recv_buffer,int length)
{
	length=read(fd,recv_buffer,length);
	return length;
}
void print_current_time_with_ms (void)
{
    long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = spec.tv_nsec / 1.0e6; // Convert nanoseconds to milliseconds

    printf("\nCurrent time: %.2f seconds since the Epoch\n",s*1000+ms);
}
double getMyTime(){
 struct timespec ts1, tw1; // both C11 and POSIX
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1); // POSIX
    clock_gettime(CLOCK_REALTIME, &tw1); // POSIX; use timespec_get in C11
 
    double posix_dur = (1000.0*ts1.tv_sec + 1e-6*ts1.tv_nsec);
    double posix_wall = 1000.0*tw1.tv_sec + 1e-6*tw1.tv_nsec;
 
//    printf("CPU time used (per clock_gettime()): %.2f ms\n", posix_dur);
    printf("Wall time passed: %.2f ms\n", posix_wall);
	return posix_wall;
}
float a[3],w[3],Angle[3],h[3];
struct IMUdata{
	double ts;
	float a[3];
	float w[3];
	IMUdata(double t, float* acc, float* gyro){ts = t; a[0] = acc[0]; a[1] = acc[1]; a[2] = acc[2]; w[0] = gyro[0]; w[1] = gyro[1]; w[2] = gyro[2];}
};
//std::vector<IMUdata> allIMUdata;
double curTime;
float aa0,aa1,aa2;
float ww0,ww1,ww2;
float AA0,AA1,AA2;
bool newData = false;
void ParseData(char chr)
{
		static char chrBuf[100];
		static unsigned char chrCnt=0;
		signed short sData[4];
		unsigned char i;
		
		time_t now;
		chrBuf[chrCnt++]=chr;
		if (chrCnt<11) return;
		
		if ((chrBuf[0]!=0x55)||((chrBuf[1]&0x50)!=0x50)) {printf("Error:%x %x\r\n",chrBuf[0],chrBuf[1]);memcpy(&chrBuf[0],&chrBuf[1],10);chrCnt--;return;}
		
		memcpy(&sData[0],&chrBuf[2],8);
		struct timeval  tv;
		
		switch(chrBuf[1])
		{
		    case 0x51:
			    for (i=0;i<3;i++) a[i] = (float)sData[i]/32768.0*16.0;
			    time(&now);
			    //printf("\r\nT:%s a:%6.3f %6.3f %6.3f ",asctime(localtime(&now)),a[0],a[1],a[2]);
			    curTime = getMyTime();
			    aa0 = (a[0]);
			    aa1 = (a[1]);
			    aa2 = (a[2]);
			    newData = true;
			    break;
		    case 0x52:
			    for (i=0;i<3;i++) w[i] = (float)sData[i]/32768.0*2000.0;
			    ww0 = w[0];
			    ww1 = w[1];
			    ww2 = w[2];
			    printf("w:%7.3f %7.3f %7.3f ",w[0],w[1],w[2]);					
			    
			    break;
		    case 0x53:
			    for (i=0;i<3;i++) Angle[i] = (float)sData[i]/32768.0*180.0;
			    printf("A:%7.3f %7.3f %7.3f ",Angle[0],Angle[1],Angle[2]);
			    AA0 = (Angle[0]);
			    AA1 = (Angle[1]);
			    AA2 = (Angle[2]);
			    break;
		    case 0x54:
			    for (i=0;i<3;i++) h[i] = (float)sData[i];
			    printf("h:%4.0f %4.0f %4.0f ",h[0],h[1],h[2]);
			    
			    break;
		}		
		chrCnt=0;
//		IMUdata curData(curTime, a, w);
		//allIMUdata.push_back(curData);	
		//std::string msg = "{time:" + std::to_string(curTime)
		 //+ ",acc:[" + std::to_string(a[0]) + ","+ std::to_string(a[1]) + "," + std::to_string(a[2]) + "],"
		 //+ ",gyro:[" + std::to_string(w[0]) + ","+ std::to_string(w[1]) + "," + std::to_string(w[2]) + "],"
		 //+"}";
		 
}

int main(int argc, char* argv[])
{
    // server and port
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    // connect the server
    if (argc != 3) { 
	printf("Usage: server port\n");
	exit(1);
    }
    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("socket");

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp==0) error("Unknown host");

    bcopy((char *)hp->h_addr, 
	(char *)&server.sin_addr,
	hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length=sizeof(struct sockaddr_in);
	
	// imu part
    char r_buf[1024];
    bzero(r_buf,1024);

    fd = uart_open(fd,"/dev/ttyUSB0");/*串口号/dev/ttySn,USB口号/dev/ttyUSBn */ 
    if(fd == -1)
    {
        fprintf(stderr,"uart_open error\n");
        exit(EXIT_FAILURE);
    }

    if(uart_set(fd,BAUD,8,'N',1) == -1)
    {
        fprintf(stderr,"uart set failed!\n");
        exit(EXIT_FAILURE);
    }

	FILE *fp;
	fp = fopen("Record.txt","w");
    while(1)
    {
        ret = recv_data(fd,r_buf,44);
        if(ret == -1)
        {
            fprintf(stderr,"uart read failed!\n");
	    close(sock);
// write the data
//std::ofstream outputFile;
//outputFile.open("IMU" + std::to_string(getMyTime())+".txt");
//outputFile << "timestampe,a.x,a.y,a.z,w.x,w.y,w.z\n";
//for(int i = 0; i < allIMUdata.size(); i++){
	//outputFile << allIMUdata[i].ts << "," << allIMUdata[i].a[0] << "," << allIMUdata[i].a[1] << "," << allIMUdata[i].a[2] << "," << allIMUdata[i].w[0] << "," << allIMUdata[i].w[1] << "," << allIMUdata[i].w[2] << "\n";
//}
//outputFile.close();
            exit(EXIT_FAILURE);
        }
		for (int i=0;i<ret;i++) {
		    fprintf(fp,"%2X ",r_buf[i]);
		    //printf("ParseData:");
		    ParseData(r_buf[i]);
		}
		if(newData){
		    
		printf("\nmy ww:%7.3f %7.3f %7.3f ",ww0,ww1,ww2);
		printf("my aa:%7.3f %7.3f %7.3f \n",aa0,aa1,aa2);
		
		bzero(udp_buffer,sizeof(double)*4);
		memcpy(udp_buffer, &curTime, sizeof(double));
		
		//memcpy(udp_buffer, &curTime, sizeof(double));
		
		memcpy(udp_buffer+8, &aa0, sizeof(float));
		memcpy(udp_buffer+8+4, &aa1, sizeof(float));
		memcpy(udp_buffer+8+4*2, &aa2, sizeof(float));
		
		memcpy(udp_buffer+8+4*3, &ww0, sizeof(float));
		memcpy(udp_buffer+8+4*4, &ww1, sizeof(float));
		memcpy(udp_buffer+8+4*5, &ww2, sizeof(float));
	    
		//memcpy(udp_buffer+sizeof(double)+sizeof(double)*3, gyro, sizeof(double)*3);
		n=sendto(sock,udp_buffer,
		    sizeof(double)*4,0,(const struct sockaddr *)&server,length);
		if (n < 0) error("Sendto");
		newData = false;
	    }
        usleep(1000);
    }

    ret = uart_close(fd);
    if(ret == -1)
    {
        fprintf(stderr,"uart_close error\n");
	close(sock);
        exit(EXIT_FAILURE);
    }
    close(sock);
    exit(EXIT_SUCCESS);
}
