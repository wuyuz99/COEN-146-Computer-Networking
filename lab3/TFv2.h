/*lab3 header
 *Yuzheng Wu
 *Thursday 9:15-12:00*/
#ifndef TFv2_h
#define TFv2_h

#define SIZE 10

typedef struct 
{
    int seq_ack;
    int length;
    int cksum;
}HEADER;

typedef struct 
{
    HEADER header;
    char data[SIZE];
}PACKET;
int checksum(PACKET);
#endif
