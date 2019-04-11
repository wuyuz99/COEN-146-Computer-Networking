#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<netinet/in.h>
#include<time.h>
#include<stdbool.h>

#define INF 10000

typedef struct{
    char name[50];
    char ip[50];
    int port;
}MACHINE;
typedef struct{
    int routerId;
    int neighborId;
    int newCost;
}MESSAGE;

int main(int,char*[]);
void* linkState(void*);
void* receiveInfo(void*);
void updateInfo();
void printTable();
void updateTable(MESSAGE);
void Broadcast(MESSAGE);

int cost_matrix[4][4];
int num;
int routerId;
int sock;
MACHINE machine_list[4];
pthread_mutex_t my_mutex;
struct sockaddr_in addrs[4-1];
struct sockaddr_in serverAddr;

int main(int argc, char*argv[])
{
    if(argc!=5)
    {
	printf("incorrect arguements,expected:thisId, total#, costs, machines\n");
	exit(1);
    } 
    int i;
    int j;
    FILE* cfp;
    FILE* mfp;
    pthread_t thr1;
    pthread_t thr2;

    routerId=atoi(argv[1]);
    num=atoi(argv[2]);
    pthread_mutex_init(&my_mutex,NULL);
    memset(cost_matrix,'\0',16*sizeof(int));
    srand(time(0));

    //reading input files
    cfp=fopen(argv[3],"r");
    mfp=fopen(argv[4],"r");
    if(cfp==NULL||mfp==NULL)
    {
	printf("file open failed\n");
	exit(1);
    }
    for(i=0;i<num;i++)
    {
	for(j=0;j<num;j++)
	{
	    fscanf(cfp,"%d ",&cost_matrix[i][j]);
	}
    }
    printTable();
  
    for(i=0;i<num;i++)
    {
	fscanf(mfp,"%s %s %d\n",&machine_list[i].name,&machine_list[i].ip,&machine_list[i].port);
    }
    printf("hosts table:\n");
    for(i=0;i<num;i++)
    {
	printf("%s %s %d\n",machine_list[i].name,machine_list[i].ip,machine_list[i].port);
    }
    fclose(cfp);
    fclose(mfp);
    
    //create socket and config addresses
    if((sock=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
	printf("socket error\n");
	exit(1);
    }
    //setting up sending side
    for(i=0,j=0;i<num;i++)
    {
	if(i!=routerId)
	{
	    addrs[j].sin_family=AF_INET;
	    addrs[j].sin_port=htons(machine_list[i].port);
	    inet_pton(AF_INET,machine_list[i].ip,&addrs[j].sin_addr.s_addr);
	    j++;
	}
    }
    //setting up recving side
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(machine_list[routerId].port);
    serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(sock,(struct sockaddr*)&serverAddr,sizeof(serverAddr))!=0)
    {
	printf("bind error\n");
	exit(1);
    }
    //start three threats
    pthread_create(&thr1,NULL,&linkState,NULL);
    pthread_create(&thr2,NULL,&receiveInfo,NULL);
    updateInfo();
}


void updateTable(MESSAGE msg)
{
    int src=msg.routerId;
    int dest=msg.neighborId;
    int cost=msg.newCost;
    pthread_mutex_lock(&my_mutex);
    cost_matrix[src][dest]=cost;
    cost_matrix[dest][src]=cost;
    pthread_mutex_unlock(&my_mutex);
    printTable();
}
void Broadcast(MESSAGE msg)
{
    int i;
    pthread_mutex_lock(&my_mutex);
    for(i=0;i<3;i++)
    {
	sendto(sock,&msg,sizeof(MESSAGE),0,(struct sockaddr*)&addrs[i],sizeof(struct sockaddr));
    }
    pthread_mutex_unlock(&my_mutex);
    printf("broadcasted\n");    
}
void printTable()
{
    int i;
    int j;
    pthread_mutex_lock(&my_mutex);
    printf("cost matrix:\n");
    for(i=0;i<4;i++)
    {
	for(j=0;j<4;j++)
	{
	    printf("%-4d ",cost_matrix[i][j]);
	}
	printf("\n");
    }
    pthread_mutex_unlock(&my_mutex);
}
void updateInfo()
{
    int count=0;
    int neighborId;
    int cost;
    MESSAGE msg;
    msg.routerId=routerId;
    while(count<2)
    {
	sleep(3);
	printf("please enter change(neighbor newcost):");
	scanf("%d %d",&msg.neighborId,&msg.newCost);
	updateTable(msg);
	Broadcast(msg);
	count++;
    }
    sleep(15);
}
void* linkState(void *a)
{
    int leastDistArray[4];
    int visited[4];
    int temp;
    int i;
    int j;
    int k;
    int min;
    int minId;
    while(1)
    {
	printf("\nnew cost array:\n");
	for(i=0;i<4;i++)
	{
	    for(j=0;j<4;j++)
	    {
		visited[j]=0;
	    }//clear visited array
	    for(j=0;j<4;j++)
	    {
		leastDistArray[j]=cost_matrix[i][j];
	    }
	    visited[i]=1;
	    for(j=0;j<3;j++)
	    {
		min=INF;
		minId=-1;	
		for(k=0;k<4;k++)
		{
		    if(!visited[k]&&leastDistArray[k]<min)
		    {
			min=leastDistArray[k];
			minId=k;
		    }
		}//find minimum unvisited node	    
		visited[minId]=1;
		for(k=0;k<4;k++)
		{
		    if(!visited[k])
		    {
			temp=leastDistArray[minId]+cost_matrix[minId][k];
			if(temp<leastDistArray[k])
			{
			    leastDistArray[k]=temp;
			}
		    }
		}
	    }
	    for(k=0;k<4;k++)
	    {
	        printf("%d ",leastDistArray[k]);
	    }
	    printf("\n");
	}
	sleep(rand()%15);
    }

}
void* receiveInfo(void *a)
{
    MESSAGE msg;
    while(1)
    {
	recvfrom(sock,&msg,sizeof(MESSAGE),0,NULL,NULL);
	printf("message recieved\n");
	updateTable(msg);
    }
}
