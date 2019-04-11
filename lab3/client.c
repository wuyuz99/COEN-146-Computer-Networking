/*lab2 client
 * Yuzheng Wu
 * Thursday 9:15-12:00*/
#include"TFv2.h"
#include<stdlib.h>
#include<stdio.h>
#include<netinet/in.h>
#include<string.h>
#include<time.h>

PACKET makepacket(char* buff,int n);
PACKET remake(PACKET pkt);
int sendpacket(int sock,PACKET pkt,struct sockaddr_in serverAddr,socklen_t addr_size);
int seq=0;

int main(int argc,char* argv[])
{
    int sock;
    int n;
    char buff[SIZE];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    FILE *inf;
    PACKET pkt;    
    //check num of areguements
    if(argc!=5)
    {
	printf("expecting 4 arguements:port, ip, infile, outfile\n");
	return(1);
    }
    //initialize
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(atoi(argv[1]));
    inet_pton(AF_INET, argv[2], &serverAddr.sin_addr.s_addr);
    memset(serverAddr.sin_zero,'\0',sizeof(serverAddr.sin_zero));
    memset(&pkt,'\0',sizeof(pkt));
    addr_size=sizeof(serverAddr);
    srand(time(0));
    //create socket
    if((sock=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        printf("socket error\n");
        return(1);
    }
    //open file
    inf=fopen(argv[3],"rb");
    if(inf==NULL)
    {
	printf("file open error\n");
        return(1);
    }
    pkt=makepacket(argv[4],strlen(argv[4])+1);
    sendpacket(sock,pkt,serverAddr,addr_size);
    printf("filename sent\n");
    while(!feof(inf))
    {
        n=fread(buff,1,SIZE,inf);
        pkt=makepacket(buff,n);
        sendpacket(sock,pkt,serverAddr,addr_size);	
    }   
    pkt=makepacket("",0);
    sendpacket(sock,pkt,serverAddr,addr_size);
    fclose(inf);
    printf("file transfer ended\n");
}

PACKET makepacket(char* buff,int n)
{
    PACKET pkt;
    strcpy(pkt.data,buff);
    pkt.header.seq_ack=seq;
    pkt.header.length=n;
    pkt.header.cksum=0;
    if(rand()%10<=8)
    {
        pkt.header.cksum=checksum(pkt);
    }
    return(pkt);
}
PACKET remake(PACKET pkt)
{
    return(makepacket(pkt.data,pkt.header.length));
}

int sendpacket(int sock,PACKET pkt,struct sockaddr_in serverAddr,socklen_t addr_size)
{
    int counter=0;
    PACKET rcv;
    sendto(sock,&pkt,sizeof(PACKET),0,(struct sockaddr*)&serverAddr,addr_size);	
    while((recvfrom(sock,&rcv,sizeof(PACKET),0,NULL,NULL))!=0)
    {
	if(rcv.header.seq_ack==seq)
	{
	    break;
	}
	counter++;
	printf("resending %d\n",counter);
	pkt=remake(pkt);
	sendto(sock,&pkt,sizeof(PACKET),0,(struct sockaddr*)&serverAddr,addr_size);
        if(counter==3)
	{
	    printf("3 same ack\n");
	    exit(1);
	}
    }
	seq=!seq;
}
