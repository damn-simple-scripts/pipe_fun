#define _GNU_SOURCE
#define _BSD_SOURCE

#include <sys/queue.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define READ_SIZE  (64*1024)
#define TOTAL_SIZE (1*1024*1024*1024)

#define ERR_PTR ((void*) 1337 )

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
size_t queue_length = 0;
unsigned char shutdown_reached = 0;
unsigned char shutdown_graceful = 0;

typedef struct buff_node {
		size_t buff_usage;
		unsigned char buff[READ_SIZE];

		TAILQ_ENTRY(buff_node) entries;
} buff_node_t;

TAILQ_HEAD(head_s, buff_node) queue_head;

void* task_read(void* arg){
	ssize_t bytes_got = 0;
	buff_node_t* new_elem = NULL;

	while(shutdown_reached == 0){
		new_elem = (buff_node_t*)	malloc(sizeof(buff_node_t));
		new_elem->buff_usage = 0;

		while( shutdown_reached == 0 && (queue_length*READ_SIZE) >= TOTAL_SIZE ){
			usleep(1);
		}

		bytes_got = read(0, new_elem->buff , READ_SIZE);
		if(bytes_got <= -1){
			perror("Reading failed ");
			return ERR_PTR;
		}else if(bytes_got == 0){
			shutdown_graceful = 1;
			free(new_elem), new_elem = NULL;
			perror("READ exited ");
			return NULL;
		}
		new_elem->buff_usage = (size_t) bytes_got;

		pthread_mutex_lock(&queue_mutex);
		TAILQ_INSERT_TAIL(&queue_head, new_elem, entries);
		queue_length++;
		pthread_mutex_unlock(&queue_mutex);
	}
	return NULL;
}

void* task_write(void* arg){
	buff_node_t* elem = NULL;
	ssize_t bytes_wrote = 0;

	while(shutdown_reached == 0){
		while (
				(shutdown_reached == 0) && 
				TAILQ_EMPTY(&queue_head) && 
				(shutdown_graceful == 0)
		){
			usleep(1);
		}
	
		if( TAILQ_EMPTY(&queue_head) ){
			return NULL;
		}
		pthread_mutex_lock(&queue_mutex);
		elem = TAILQ_FIRST(&queue_head);
		TAILQ_REMOVE(&queue_head, elem, entries);
		queue_length--;
		pthread_mutex_unlock(&queue_mutex);

		size_t write_offset = 0;
		for(;;){
			bytes_wrote = write(
					1, 
					(elem->buff) + write_offset, 
					elem->buff_usage - write_offset
			);
			if(bytes_wrote <= -1){
				perror("Write fail ");
				shutdown_reached = 1;
				return ERR_PTR;
			}else if(bytes_wrote == 0){
				perror("Output was closed ");
				shutdown_reached = 1;
				return ERR_PTR;
			}else if( ((size_t)bytes_wrote)+write_offset != elem->buff_usage){
				write_offset += ((size_t)bytes_wrote);
			}else{
				break;
			}
		}
		free(elem);
	}
	return NULL;
}

int main(int argc, const char *argv[])
{
	pthread_t read_thread;
	pthread_t write_thread;
	TAILQ_INIT(&queue_head);
	pthread_create (&read_thread, NULL, task_read, NULL);
	pthread_create (&write_thread, NULL, task_write, NULL);

	pthread_join(read_thread, NULL);
	pthread_join(write_thread, NULL);

	return 0;
}
