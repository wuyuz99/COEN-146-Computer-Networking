/*lab3 server
 *Yuzhen Wu
 *Thursday 9:15-12:00*/
#include "TFv2.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

PACKET makepacket(char *buff,int n);
PACKET recieve_packet(struct sockaddr_storage *serverStorage,socklen_t *addr_size);
int sock;
int seq=0;

int main(int argc, char *argv[])
{
    char fname[SIZE];
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    socklen_t client_addr_size;
    FILE *ofp;
    PACKET pkt;
    //check number of inputs
    if(argc!=2)
    {
        printf("port number needed\n");
        return(1);
    }
    //initialize values
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons ((short)atoi (argv[1]));
    serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    memset((char*)serverAddr.sin_zero,'\0',sizeof(serverAddr.sin_zero));
    memset(&pkt,'\0',sizeof(PACKET));  
    addr_size = sizeof (serverStorage);
    srand(time(0));
    //create socket
    if((sock=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        printf ("socket error\n");
        return(1);
    }
    //bind socket
    if(bind(sock,(struct sockaddr*)&serverAddr,sizeof(serverAddr))!=0)
    {
        printf("bind error\n");
        return(1);
    }
    pkt=recieve_packet(&serverStorage,&addr_size);
    memcpy(fname,pkt.data,SIZE);
    fname[pkt.header.length]='\0';
    if((ofp=fopen(fname,"wb"))==NULL)
    {
        printf("file open failed\n");
        return(1);
    }

    while(1)
    {
        pkt=recieve_packet(&serverStorage,&addr_size);
	if(pkt.header.length==0)
	{
	    printf("file recieved\n");
	    fclose(ofp);
	    break;
	}
	fwrite(pkt.data,1,pkt.header.length,ofp);
    }	
}
PACKET makepacket(char *buff,int n)
{
    PACKET pkt;
    strcpy(pkt.data,buff);
    pkt.header.seq_ack=seq;
    pkt.header.length=n;
    pkt.header.cksum=0;
    pkt.header.cksum=checksum(pkt);
    return(pkt);
}
PACKET recieve_packet(struct sockaddr_storage *serverStorage,socklen_t *addr_size)
{
    int cksum;
    char buff[10]="";
    PACKET pkt;
    PACKET ack;
    while(1)
    {
        recvfrom(sock,&pkt,sizeof(PACKET),0,(struct sockaddr*)serverStorage,addr_size);
	cksum=pkt.header.cksum;
	pkt.header.cksum=0;
	if((rand()%10)<8)
	{
	    if(cksum!=checksum(pkt))
	    {
		printf("checksum failed\n");
		seq=!seq;
		ack=makepacket(buff,0);
		seq=!seq;
		sendto(sock,&ack,sizeof(PACKET),0,(struct sockaddr*)serverStorage,*addr_size);
	    
	    }else{
		ack=makepacket(buff,0);
		sendto(sock,&ack,sizeof(PACKET),0,(struct sockaddr*)serverStorage,*addr_size); 
		break;
	    }
	}else{
	    printf("packet unsent\n");
	}
    }
    
    seq=!seq;
    return(pkt);
}
