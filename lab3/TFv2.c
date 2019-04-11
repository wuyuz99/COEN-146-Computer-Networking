/*lab3 checksum
 *Yuzheng Wu
 *Thursday 9:15-12:00*/
#include "TFv2.h"

int checksum(PACKET pkt)
/*{
    int sum=0;
    int i;
    char *p=(char*)&pkt;
    for(i=0;i<pkt.header.len+12;i++,p++)
    {
	sum=sum^(*(p));
    }	
    return sum;
}*/
{
    int i;
    char cs=0;
    char*p=(char*)&pkt;
    for(i=0;i<sizeof(PACKET);i++)
    {
	cs=cs^*p;
	++p;
    }
    return((int)cs);
}
    
