/* Hunter Kitts
 * LabA chat_server.c
 * chat server using pthreads
 * clients connect with nc
 * usage: chat-server port Chat-Room-Names ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>

#include "fields.h"
#include "jrb.h"
#include "dllist.h"
#include "jval.h"
#include "sockettome.h"

typedef struct{
	char *name;
	Dllist chat_list;
	Dllist client_list;
	pthread_mutex_t *lock;
	pthread_cond_t *output_cond;
} Room_Thread;

typedef struct{
	char *name;
	int fd;
	FILE *fin, *fout;
	Room_Thread *s;
} Client;

JRB rooms;	/* list of chat rooms 
			 * so the clients can print out all the chat rooms
			 */

void sig_pipe_handler(int dummy){
	signal(SIGPIPE, sig_pipe_handler);
}

/* Prints the rooms chat_list when a signal is given
 * prints to all clients */
void *room_output(void *arg){

	Dllist dllist_node;		/* Used to get clients */
	Room_Thread *rt;		/* Threads struct for rooms */
	Client *c;				/* Temp holds client to print to it */

	rt = (Room_Thread *) arg;

	pthread_mutex_lock(rt->lock);

	while(1){
		/* if the list is added on to
		 * traverse the client list 
		 * print to every client */
		while(!dll_empty(rt->chat_list)){
			dll_traverse(dllist_node, rt->client_list){
				c = (Client *) jval_v(dllist_node->val);
				fputs(rt->chat_list->flink->val.s, c->fout);
				fflush(c->fout);
			}
			dll_delete_node(rt->chat_list->flink);
		}
		pthread_cond_wait(rt->output_cond, rt->lock);
	}
	return NULL;
}

/* Gets initial info from client
 * Name and Room - only time fputs is used
 * Then all other fputs to this client are done 
 * from the room
 */
void *client_thread(void *arg){
	JRB jrb_node;
	Dllist tmp_dllist;
	Dllist dll_node;
	Client *c;
	Client *tmp_c;
	Room_Thread *rt;
	char name[100];
	char room_name[300];
	char print_room[1000];
	char input[1000];

	/* Set up client */
	c = (Client *) arg;
	c->fin = fdopen(c->fd, "r");
	c->fout = fdopen(c->fd, "w");

	/* print chat rooms
	 * prints all the chat rooms and the clients
	 * currently in the room */
	fputs("Chat Rooms:\n\n", c->fout);
	jrb_traverse(jrb_node, rooms){
		rt = (Room_Thread *) jrb_node->val.v;
		pthread_mutex_lock(rt->lock);		/* lock room */
		strcpy(print_room, rt->name);
		strcat(print_room, ":");

		if(!dll_empty(rt->client_list)){
			dll_traverse(tmp_dllist, rt->client_list){
				tmp_c = (Client *) jval_v(tmp_dllist->val);
				strcat(print_room, " ");
				strcat(print_room, tmp_c->name);
			}
		}
		strcat(print_room, "\n");

		fputs(print_room, c->fout);
		pthread_mutex_unlock(rt->lock);		/* unlock room */
	}

	/* get clients name */
	fputs("\nEnter your chat name (no spaces):\n", c->fout);
	fflush(c->fout);
	
	if(fgets(name, 100, c->fin) == NULL){
		close(c->fd);
		fclose(c->fin);
		fclose(c->fout);
		return NULL;
	}
	size_t len = strlen(name);
	if(len > 0 && name[len - 1] == '\n'){
		name[len - 1] = '\0';
	}
	c->name = strdup(name);

	/* get the room name the client will chat in */
	fputs("Enter chat room:\n", c->fout);
	fflush(c->fout);

	if(fgets(room_name, 300, c->fin) == NULL){
		close(c->fd);
		fclose(c->fin);
		fclose(c->fout);
		free(c->name);
		return NULL;
	}
	len = strlen(room_name);
	if(len > 0 && room_name[len - 1] == '\n'){
		room_name[len - 1] = '\0';
	}

	/* find that chat room in the tree */
	jrb_node = jrb_find_str(rooms, room_name);
		rt = (Room_Thread *) jrb_node->val.v;
		c->s = rt;

	/* tell the room and other clients that another client
	 * has joined */
	/* lock the room */
	pthread_mutex_lock(c->s->lock);
	char name_added[150];
	strcpy(name_added, c->name);
	strcat(name_added, " has joined\n");
	/* add the "has joined" phrase and the client to the rooms dllists */
	dll_append(c->s->chat_list, new_jval_s(strdup(name_added)));
	dll_append(c->s->client_list, new_jval_v(c));
	/* signal the room and unlock the room */
	pthread_cond_signal(c->s->output_cond);	
	pthread_mutex_unlock(c->s->lock);

	/* look for clients chat */
	/* same steps apply as before
	 * lock room
	 * add to the dllist
	 * signal room
	 * unlock room */
	while(fgets(input, 1000, c->fin) != NULL){
		char name_and_input[1500];
		strcpy(name_and_input, c->name);
		strcat(name_and_input, ": ");
		strcat(name_and_input, input);
		pthread_mutex_lock(c->s->lock);
		dll_append(c->s->chat_list, new_jval_s(strdup(name_and_input)));
		pthread_cond_signal(c->s->output_cond);
		pthread_mutex_unlock(c->s->lock);
	}

	/* if the EOF is caught */
	/*lock the room */
	pthread_mutex_lock(c->s->lock);
	strcpy(name_added, c->name);
	strcat(name_added, " has left\n");
	/* add "has left" phrase and remove client from rooms list */
	dll_append(c->s->chat_list, new_jval_s(strdup(name_added)));
	Dllist tmp;
	dll_traverse(dll_node, c->s->client_list){
		if(jval_v(dll_node->val) == c){
			tmp = dll_node;
		}
	}
	tmp_c = (Client *) jval_v(tmp->val);
	dll_delete_node(tmp);
	/* signal the room and unlock the room */
	pthread_cond_signal(c->s->output_cond);
	pthread_mutex_unlock(c->s->lock);

	/* close the clients info */
	close(c->fd);
	fclose(c->fin);
	fclose(c->fout);
	free(c->name);

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t ct;
	Client *c;
	Room_Thread *rt;
	int port;
	int socket;
	int fd;
	int i;
	int j = 0;


	//error check usage
	if(argc < 3){
		printf("usage: chat_server port Chat_Room_Names ...\n");
		exit(0);
	}

	//get port number & error check
	port = atoi(argv[1]);
	if(port < 8000){
		printf("Must use a port >= 8000\n");
		exit(0);
	}

	/* room threads */
	pthread_t rtids[argc -2];	

	/* tree of room threads */
	rooms = make_jrb();

	/* make the room threads and call room_output for each room */
	for(i = argc-1; i > 1; i--){
		rt = (Room_Thread *) malloc(sizeof(Room_Thread));
		rt->name = strdup(argv[i]);
		strcat(rt->name, "\0");
		rt->chat_list = new_dllist();
		rt->client_list = new_dllist();
		jrb_insert_str(rooms, strdup(rt->name), new_jval_v(rt));
		rt->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(rt->lock, NULL);
		rt->output_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
		pthread_cond_init(rt->output_cond, NULL);
		pthread_create(&rtids[j], NULL, room_output, rt);
		j++;
	}

	signal(SIGPIPE, sig_pipe_handler);

	//serve socket
	socket = serve_socket(port);

	/* loop waiting for client to accept_connection */
	while(1){
		fd = accept_connection(socket);
		/* make the client and each client calls client_thread */
		c = (Client *) malloc(sizeof(Client));
		c->fd = fd;
		pthread_create(&ct, NULL, client_threald, c);
	}
	return 0;
}
