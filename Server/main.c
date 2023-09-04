
#include "main.h"

#include <mqueue.h>
#include <unistd.h>  //For sleep



char mem_string[MAX_CHAT_STRING][MAX_CHAT_STR_LENGTH];
unsigned char count_string;


char mq_server_name[MQ_NAME_LENGTH] = "/server_chat_comon_room";
//mqd_t desc_server_in;
//mqd_t desc_server_out;
mqd_t desc_server;

unsigned char count_user;
char mq_name[MAX_NUMBER_USER][MQ_NAME_LENGTH];
char mq_name_in[MAX_NUMBER_USER][MQ_NAME_LENGTH];
char mq_name_out[MAX_NUMBER_USER][MQ_NAME_LENGTH];
mqd_t desc_user_in[MAX_NUMBER_USER];
mqd_t desc_user_out[MAX_NUMBER_USER];

struct mq_attr mqattrs; /* Attributes of created queues */

int main(void)
{
	count_user = 0;
	count_string = 0;
 	/* Let's create a message queue */
	mqattrs.mq_flags = O_NONBLOCK;
//	mqattrs.mq_flags = 0;
	mqattrs.mq_maxmsg = MQ_MSGS_MAX;
	mqattrs.mq_msgsize = MAX_LEN_STRING /* sizeof (int) */;
	mqattrs.mq_curmsgs = 0;

	if((desc_server = mq_open(mq_server_name, O_RDONLY | /*O_WRONLY | */O_CREAT | O_NONBLOCK /**/, 0660, &mqattrs)) == (mqd_t) (-1)){perror ("MQ_OPEN");return (-1);}

 	while(1)
 	{
	  	ssize_t msg_size;
		char temp_string[MAX_LEN_STRING];
		msg_size = mq_receive(desc_server, temp_string, MSG_INT_SIZE, NULL);
		if(0 < msg_size)
		{
			if (strcmp (temp_string, "exit")==0)
			{
			
			    for(unsigned int i = 0; i < count_user; ++i)
			    {
//			    	if(i != j)
//			    	{
				if(0 != mq_send(desc_user_out[i], temp_string, strlen(temp_string) + 1, 0)){perror ("MQ_SEND");return (-1);}
//				}
			    }
			
			    break;
			}
			
			strncpy(mq_name[count_user], temp_string,MAX_LEN_NIC_NAME);
			sprintf (mq_name_in[count_user], "%s_in", mq_name[count_user]);
			printf("%s\n", mq_name_in[count_user]);
			if((desc_user_in[count_user] = mq_open(mq_name_in[count_user], O_RDONLY | O_CREAT | O_NONBLOCK, 0660, &mqattrs)) == (mqd_t) (-1)){perror ("MQ_OPEN");return (-1);}
			sprintf (mq_name_out[count_user], "%s_out", mq_name[count_user]);
			printf("%s\n", mq_name_out[count_user]);
			if((desc_user_out[count_user] = mq_open(mq_name_out[count_user], O_WRONLY | O_CREAT | O_NONBLOCK, 0660, &mqattrs)) == (mqd_t) (-1)){perror ("MQ_OPEN");return (-1);}
			sleep (4);
			for(unsigned int i = 0; i < count_string; ++i)
			{
			usleep (200);
//				printf("mem");
//				printf("%s\n", mem_string[i]);
				if(0 != mq_send(desc_user_out[count_user], mem_string[i], strlen(mem_string[i]) + 1, 0)){perror ("MQ_SEND");return (-1);}
			}
			
//			for(unsigned int i = 0; i < count_string; ++i)
//			{
//				sprintf (temp_string, "%s:", mq_name[count_user]);
//				if(0 != mq_send(desc_user_out[count_user], temp_string, strlen(temp_string) + 1, 0)){perror ("MQ_SEND");return (-1);}
//			}
			++count_user;
			
			usleep (1000);
			temp_string[0]='>';temp_string[1]=0;
			if(0 != mq_send(desc_user_out[count_user - 1], temp_string, strlen(temp_string) + 1, 0)){perror ("MQ_SEND");return (-1);}
			for(unsigned int i = 0; i < count_user; ++i)
			{
			usleep (200);
//			    if(i != j)
//			    {
				sprintf (temp_string, "%s:", mq_name[i]);
//				printf ("\n  hold \n");
//				printf ("%s", temp_string);
//				printf ("\n  hold \n");
				if(0 != mq_send(desc_user_out[count_user - 1], temp_string, strlen(temp_string) + 1, 0)){perror ("MQ_SEND");return (-1);}
//			    }
			}
			temp_string[0]='<';temp_string[1]=0;
			if(0 != mq_send(desc_user_out[count_user - 1], temp_string, strlen(temp_string) + 1, 0)){perror ("MQ_SEND");return (-1);}
			
	  	}
		

		/////////////////////////////////////////////////////////////////////////

		for(unsigned int i = 0; i < count_user; ++i)
		{
		
			msg_size = mq_receive(desc_user_in[i], temp_string, MSG_INT_SIZE, NULL);
			if(0 < msg_size)
			{
				printf("%s\n", temp_string);
				strncpy(mem_string[count_string], temp_string, MAX_CHAT_STR_LENGTH);
				++count_string;
				
				for(unsigned int j = 0; j < count_user; ++j)
				{
					if(i != j)
					{
					printf("num = %d str = %s\n", j, temp_string);
						if(0 != mq_send(desc_user_out[j], temp_string, strlen(temp_string) + 1, 0)){perror ("MQ_SEND");return (-1);}
					}
					else
					{
					char temp_string_2[MAX_LEN_STRING];
			temp_string_2[0]='>';temp_string_2[1]=0;
			if(0 != mq_send(desc_user_out[j], temp_string_2, strlen(temp_string_2) + 1, 0)){perror ("MQ_SEND");return (-1);}
						for(unsigned int k = 0; k < count_user; ++k)
						{
						usleep (200);
							sprintf (temp_string_2, "%s:", mq_name[k]);
							if(0 != mq_send(desc_user_out[j], temp_string_2, strlen(temp_string_2) + 1, 0)){perror ("MQ_SEND");return (-1);}
						}
			temp_string_2[0]='<';temp_string_2[1]=0;
			if(0 != mq_send(desc_user_out[j], temp_string_2, strlen(temp_string_2) + 1, 0)){perror ("MQ_SEND");return (-1);}
					}
				}
			}
		}

        
 	}
 
 
	if(mq_close (desc_server) == -1) {perror ("Server: mq_close");return (-1);}
	if(mq_unlink (mq_server_name) == -1) {perror ("Server: mq_unlink");return (-1);}
 	
 	
	for(unsigned int i = 0; i < count_user; ++i)
	{
		char temp_string[MAX_LEN_STRING];
		if(mq_close (desc_user_in[i]) == -1) {perror ("Client: mq_close");return (-1);}
		if(mq_close (desc_user_out[i]) == -1) {perror ("Client: mq_close");return (-1);}
		sprintf (temp_string, "%s_in", mq_name[i]);
		if(mq_unlink (temp_string) == -1) {perror ("Client: mq_unlink");return (-1);}
		sprintf (temp_string, "%s_out", mq_name[i]);
		if(mq_unlink (temp_string) == -1) {perror ("Client: mq_unlink");return (-1);}
	}
 	
 	
 	printf("The End\n");
	return 0;
}








