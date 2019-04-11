/*Yuzheng Wu
 *Thursday 9:15-12:00
 */
#include<stdio.h>
#include<assert.h>
#define SIZE 10

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
	printf("expected two arguements\n");
	return(-1);
    }

    FILE* inf;
    FILE* of;
    char buff[SIZE];
    size_t count=0;
    
    inf=fopen(argv[1], "rb");
    of=fopen(argv[2], "wb");
    assert(inf!=NULL && of!=NULL);

    while(!feof(inf))
    {
	count=fread(buff,1,SIZE,inf);
	fwrite(buff,1,count,of);
    }

    fclose(inf);
    fclose(of);
    
    return(0);
}
