
#include <stdio.h>
#include <string.h>



#define MAX_LEN_NIC_NAME	32

#define MAX_LEN_NAME 32
#define MAX_LEN_STRING	256


#define MAX_LEN_SCAN_STRING	(MAX_LEN_STRING - MAX_LEN_NIC_NAME - 4)



/* Number of integers in one message */
#define MSG_INT_SIZE 256

/* Maximum number of messages in the queue */
#define MQ_MSGS_MAX 8


struct st_control_threads_read{
//    char array_name[MAX_LEN_NAME];
    unsigned int msg_prio;
    int status;
};

