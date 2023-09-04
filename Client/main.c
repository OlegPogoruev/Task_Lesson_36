
#include "main.h"

#include <ncurses.h>


#include <pthread.h>
//#include <stdio.h>

#include <mqueue.h>


#include <unistd.h>  //For sleep


void *threads_read_queue(void *param); /* stream function */


char name_thread[MAX_LEN_NAME] = "User_";

const char name_server[32] = "/server_chat_comon_room";
//char string_input[MAX_LEN_STRING];


struct st_control_threads_read control_threads_read_queue;

mqd_t desc_server;
struct mq_attr mqattrs; /* Attributes of created queues */

mqd_t desc_out;
mqd_t desc_in;

char nic_name[MAX_LEN_NIC_NAME];




void get_input_string(char *str_in);
void gluing_strings(char* str_com, char* str_1, char* str_2);






/////////////////////////////////////
WINDOW *win_user;// Shows current directory left
WINDOW *win_chat;// Shows current directory right
WINDOW *type_win;// Shows current directory right

void init_windows(void);
WINDOW *create_newwin(int height, int width, int starty, int startx);
void refreshWindows(void);

int maxx, maxy;
/////////////////////////////////////




int main(void)
{
	char temp_string[MAX_LEN_STRING];

 	printf("Enter nic name (max 20 char):\n");
 	{
	 	char temp_array[20];
		scanf("%20s", temp_array);
		sprintf (nic_name, "/%s_%d", temp_array, getpid());
//		printf("nic_name = %s\n", nic_name);
	}
// 	while(1){scanf("%256s", temp_string);printf("%s\n", temp_string);if (strcmp (temp_string, "exit")==0){return 0;}}
	

	

 //////////////////////////////////////////////////////////////////////////////////////////////
 	/* Let's create a message queue 
	mqattrs.mq_flags = O_NONBLOCK;
	mqattrs.mq_maxmsg = MQ_MSGS_MAX;
	mqattrs.mq_msgsize = MAX_LEN_STRING;
	mqattrs.mq_curmsgs = 0;
	*/

	if((desc_server = mq_open(name_server, O_WRONLY)) == (mqd_t) (-1)){endwin();perror ("MQ_OPEN");return (-1);}
	sleep (1);
	if(0 != mq_send(desc_server, nic_name, strlen(nic_name) + 1, 0)){endwin();perror ("MQ_SEND");return (-1);}
	
	
	
	printf("WAIT\n");
	sleep (2);
/////////////////////////////////////////////////////////////////////////////////////////////////
	
    WINDOW * point_window;
    point_window = initscr();

    initscr();
//    noecho();
    curs_set(0);
    start_color();

    getmaxyx(stdscr, maxy, maxx);// Get Size of terminal
//    maxy = maxy - 2;// Save last two rows for status_win
    
    init_windows();// Make the two windows side-by-side and make the status window
    refreshWindows();// Draw borders and refresh windows
    
//    while(1);
/////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	sprintf (temp_string, "%s_in", nic_name);
//	printf("%s\n", temp_string);
	if((desc_out = mq_open(temp_string, O_WRONLY)) == (mqd_t) (-1)){endwin();perror ("MQ_OPEN");return (-1);}
	sprintf (temp_string, "%s_out", nic_name);
//	printf("%s\n", temp_string);
	if((desc_in = mq_open(temp_string, O_RDONLY)) == (mqd_t) (-1)){endwin();perror ("MQ_OPEN");return (-1);}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
	pthread_t tid_read_queue;	/* thread ID */
	pthread_attr_t attr_read_queue; /* thread attributes */
 
 	control_threads_read_queue.status = 0;
	pthread_attr_init(&attr_read_queue);							/* get default attribute values */
	if(0 != pthread_create(&tid_read_queue, &attr_read_queue, threads_read_queue, name_thread))	/* create a new thread */
 	{endwin();perror ("PTHREAD_CREATE");return (-1);}
 //////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////
  	sleep (1);
  	wmove(type_win,1,1);
  	
  	
  
 	while(1)
 	{
 	    char temp_array[MAX_LEN_STRING];
 	    get_input_string(temp_array);
// 	    scanf("%256s", temp_string);
// 	    printf("%s\n", temp_string);

 	    if(0 != control_threads_read_queue.status)
 	    {
 	    	if(0 != mq_send(desc_out, temp_array, strlen (temp_array) + 1, 0)){endwin();perror ("MQ_SEND");return (-1);}
 	    	break;
 	    }
 	    
            if (strcmp (temp_array, "exit")==0)
            {
            	if(0 != mq_send(desc_server, temp_array, strlen(temp_array) + 1, 0)){endwin();perror ("MQ_SEND");return (-1);}
            	control_threads_read_queue.status = 1;
                break;
            }
            
	    gluing_strings(temp_string, nic_name, temp_array);

/*
	    for(unsigned int i = 0; i < strlen (temp_array); ++i)
	    {printf("\b");}
	    printf("%s\n", temp_string);
*/


 //   	    wclear(win_user);
 //   	    box(win_user,0,0);
 //   	    wrefresh(win_user);
//	    wmove(win_user,1,0);

	    wprintw(win_chat, "%s\n", temp_string);
	    box(win_chat,0,0);
    	    wrefresh(win_chat);
    	    
    	    wclear(type_win);
    	    box(type_win,0,0);
    	    wrefresh(type_win);
    	    wmove(type_win,1,1);

	    if(0 != mq_send(desc_out, temp_string, strlen (temp_string) + 1, 0)){endwin();perror ("MQ_SEND");return (-1);}
 	    
 	    

 	}
 
 
 
 	
 	pthread_join(tid_read_queue,NULL);		/* waiting for the thread to complete execution */
 	
 	endwin();
	printf("The End\n");
	return 0;
}


/* Control passes streaming function */
void *threads_read_queue(void *param) 
{
  int i;
  char temp_string_2[MAX_LEN_STRING];
//  printf("in threads\n");

  while(0 == control_threads_read_queue.status)
  {
  
  	ssize_t msg_size;
  
	msg_size = mq_receive(desc_in, temp_string_2, MAX_LEN_STRING, NULL);
	if(0 < msg_size)
	{
	    if (strcmp (temp_string_2, "exit")==0)
            {
            	control_threads_read_queue.status = 1;
                break;
            }
            
            
            if(('<' == temp_string_2[strlen(temp_string_2) - 1]) && (0 == temp_string_2[strlen(temp_string_2) + 0]))
            {
		    box(win_user,0,0);
		    wrefresh(win_user);
		    
            }
            else
            {
		    if(('>' == temp_string_2[strlen(temp_string_2) - 1]) && (0 == temp_string_2[strlen(temp_string_2) + 0]))
		    {
	   	    wclear(win_user);
	 //   	    box(win_user,0,0);
	 //   	    wrefresh(win_user);
		    wmove(win_user,1,0);
		    }
		    else
		    {
			    if((':' == temp_string_2[strlen(temp_string_2) - 1]) && (0 == temp_string_2[strlen(temp_string_2) + 0]))
			    {
			      	    wprintw(win_user, "%s\n", temp_string_2);
	//			    box(win_user,0,0);
	//		    	    wrefresh(win_user);
			    }
			    else
			    {
			      	    wprintw(win_chat, "%s\n", temp_string_2);
				    box(win_chat,0,0);
			    	    wrefresh(win_chat);
			    }
		    }
	    }
  	}
  
//	sleep (1);
  }
  
//printf("threads End\n");

  pthread_exit(0);
}


void get_input_string(char *str_in)
{
    int ch = 0;
    int temp_count = 0;


    while ((ch = wgetch(type_win)) != '\n')
    {
    	addch(ch);
        (*(str_in + temp_count)) = ch;
        ++temp_count;
        if(temp_count > MAX_LEN_SCAN_STRING)
        {
//          (*(str_in + temp_count)) = '\n';
//          ++temp_count;
          break;
        }
    }
//    (*(str_in + temp_count)) = '\n';
    (*(str_in + temp_count)) = 0;




/*
    int ch = 0;
    int temp_count = 0;
    while ((ch = getchar()) != '\n')
    {
        (*(str_in + temp_count)) = ch;
        ++temp_count;
        if(temp_count > MAX_LEN_SCAN_STRING)
        {
//          (*(str_in + temp_count)) = '\n';
//          ++temp_count;
          break;
        }
    }
//    (*(str_in + temp_count)) = '\n';
    (*(str_in + temp_count)) = 0;
*/    
}

void gluing_strings(char* str_com, char* str_1, char* str_2)
{
    int count_com = 0;

    while(0 != (*(str_1 + count_com)))
    {
        (*(str_com + count_com)) = (*(str_1 + count_com));
        ++count_com;
        if((MAX_LEN_STRING - 1) == count_com){(*(str_com + count_com)) = 0;return;}
    }

///////////////////////////////////////////////////////////////////
    (*(str_com + count_com)) = ':';
    ++count_com;
    if((MAX_LEN_STRING - 1) == count_com){(*(str_com + count_com)) = 0;return;}

    (*(str_com + count_com)) = ' ';
    ++count_com;
    if((MAX_LEN_STRING - 1) == count_com){(*(str_com + count_com)) = 0;return;}
///////////////////////////////////////////////////////////////////

    int count_temp = 0;
    while(0 != (*(str_2 + count_temp)))
    {
        (*(str_com + count_com)) = (*(str_2 + count_temp));
        ++count_com;
        ++count_temp;
        if((MAX_LEN_STRING - 1) == count_com){(*(str_com + count_com)) = 0;return;}
    }

    (*(str_com + count_com)) = 0;

}




/*
   Creates current_win, preview_win and status_win
*/
void init_windows(void)
{
    win_user = create_newwin(maxy - 10*maxy/100 - 1, maxx/3, 0, 0);
    win_chat = create_newwin(maxy - 10*maxy/100 - 1, 2*maxx/3, 0, maxx/3);
    type_win = create_newwin(maxy - 90*maxy/100, maxx, 90*maxy/100, 0);
//    keypad(win_left, TRUE);
//    keypad(win_right, TRUE);
//    win_activ = win_left;
    wmove(win_user,1,0);
    wmove(win_chat,1,0);
    keypad(type_win, TRUE);
    wmove(type_win,1,1);
}


/*
   Creates a new window with dimensions `height` and `width` starting at `starty` and `startx`
*/
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    return local_win;
}

/*
    Refresh ncurses windows
*/
void refreshWindows(void)
{
    box(win_user,0,0);
    box(win_chat,0,0);
    box(type_win,0,0);
    
    wrefresh(win_user);
    wrefresh(win_chat);
    wrefresh(type_win);
    wmove(type_win,1,1);
}














