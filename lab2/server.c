/*Lab2 server
 *Yuzheng Wu
 *Thursday 9:15-12:00
 */
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])
{
    int listenfd=0;
    int connfd=0;
    int n=0;
    struct sockaddr_in serv_addr;
    char buff[10];
    FILE *ouf;

    //set up
    memset(&serv_addr,'0',sizeof(serv_addr));
    memset(buff,'0',sizeof(buff));

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
	printf("socket error\n");
	return(1);
    }
    
    if((bind(listenfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr)))<0)
    {
	printf("bind error\n");
	return(1);
    }	
    if(listen(listenfd,10)<0)
    {
	printf("listen error\n");
	return(1);
    }
    while(1)
    {
	if(((connfd=accept(listenfd,(struct sockaddr*)NULL,NULL))<0))
	{
	    printf("accept error\n");
	    return(1);
	}
	n=read(connfd,buff,sizeof(buff));
	buff[n]='\0';
	ouf=fopen(buff,"wb");
	if(ouf==NULL)
	{
	    printf("file open error\n");
	    return(1);
	}
	write(connfd,"",sizeof(""));
	while((n=read(connfd,buff,sizeof(buff)))>0)
	{
	    fwrite(buff,1,n,ouf);
	}
	fclose(ouf);
	close(connfd);
	printf("file copied\n");
    }

}
