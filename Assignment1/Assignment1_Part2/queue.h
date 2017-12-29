/* Course : CSE 438 - Embedded Systems Programming
 * Assignment 1b : To implement Shared Message queues in Kernel Space
 * Team Member1 : Samruddhi Joshi  ASUID : 1213364722
 * Team Member2 : Varun Joshi 	   ASUID : 1212953337 */

#define QUEUE_LEN 10

#undef AFFINITY

/* Structure for Message */
struct message 
{
	int message_id;
	int source_id;
	unsigned long long queue_time; //queueing time/start time
	double pi_value;
};

/* Structure for DataQueue */
typedef struct dataqueue{
	int front;
	int rear;
	int full;
	struct message *message_pointer[QUEUE_LEN];
	pthread_mutex_t Mutex_DQ;
}queue;



