/*lab2 client
 * Yuzheng Wu
 * Thursday 9:15-12:00*/
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[])
{
    //check num of arguements
    if(argc!=5)
    {
	printf("expecting 4 arguements:port, ip, infile, outfile\n");
	return(1);
    }
    //initialize variables
    int sockfd=0;
    int n=0;
    char buff[10];
    struct sockaddr_in serv_addr;
    FILE *inf;

    memset(buff,'0',sizeof(buff));
    memset(&serv_addr,'0',sizeof(serv_addr));

    //open socket
    if((sockfd=socket(AF_INET, SOCK_STREAM,0))<0)
    {
	printf("socket error\n");
	return(1);
    }
    
    //set address
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(inet_pton(AF_INET,argv[2],&serv_addr.sin_addr)<=0)
    {
	printf("inet_pton error\n");
	return(1);
    }
    //connect
    if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
    {
	printf("connect error\n");
	return(1);
    }
    
    write(sockfd,argv[4],strlen(argv[4])+1);
    read(sockfd,buff,sizeof(buff));
    if(strcmp(buff,"")==0)
    {
	inf=fopen(argv[3],"rb");
	if(inf==NULL)
	{
	    printf("file open error\n");
	    close(sockfd);
	    return(1);
	}
	printf("transfer started\n");
	while(!feof(inf))
	{
	    n=fread(buff,1,10,inf);
	    write(sockfd,buff,n);
	}
	close(sockfd);
    }
}
