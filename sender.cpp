#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <pthread.h>

// using namespace qpid::messaging;
using namespace std;

typedef qpid::messaging::Connection QPID_Connection;
typedef qpid::messaging::Session QPID_Session;
typedef qpid::messaging::Sender QPID_Sender;
typedef qpid::messaging::Message QPID_Message;

typedef struct msg_sender_tag
{
    QPID_Sender * sender;
    QPID_Message * message;
}Msg_Sender;

typedef struct child_thread_arg_tag
{
    Msg_Sender * msg_sender;
    int num_of_thread;
}Child_Thread_Arg;

int thread_num = 1;
int mes_num = 1;

pthread_t * thread = NULL;
Child_Thread_Arg * child_thread_arg = NULL;

void * send_msg(void * msg_sender);
void thread_create(Child_Thread_Arg * child_thread_arg);
void thread_wait(void);

int main(int argc, char * argv[])
{
	int c;
	while ((c = getopt (argc, argv, "t:m:")) != -1)
	{
		switch (c)
		{
			case 't':
				thread_num = atoi(optarg);
				printf("thread_num:%d\n", thread_num);
				break;
			case 'm':
				mes_num = atoi(optarg);
				printf("mes_num:%d\n", mes_num);
				break;
		}
	}

	thread = (pthread_t*)calloc(thread_num, sizeof(pthread_t) );
	child_thread_arg = (Child_Thread_Arg*)calloc(thread_num, sizeof(Child_Thread_Arg));

    std::string broker_addr = string("127.0.0.1:5672");
    static const std::string topic = "amq.topic";

    QPID_Connection conn(broker_addr);

    try {

        conn.open();
        QPID_Session session = conn.createSession();
        QPID_Sender sender = session.createSender(topic);

        qpid::messaging::Message msg;

        char buffer[64];
        sprintf(buffer, "Hello, world" );
        msg.setContent(buffer);
        msg.setContentType("text/plain");

        clock_t send_begin = 0, send_end = 0;
        send_begin = clock();

        Msg_Sender msg_sender = { &sender, &msg};
		for( int i=0; i<thread_num; i++ )
		{
			child_thread_arg[i].num_of_thread = i;
			child_thread_arg[i].msg_sender = &msg_sender;
		}

        thread_create(child_thread_arg);
        thread_wait();

        send_end = clock();
		int diff_time = send_end - send_begin;
        printf("Sending %d * %d msgs, took %d clicks(%f seconds)\n", thread_num, mes_num, diff_time,((float)diff_time)/CLOCKS_PER_SEC);
        session.sync();

        conn.close();
    } catch (const std::exception& ignored) {

    }

    return 0;

}

void * send_msg(void * child_thread_arg)
{
    Msg_Sender * msg_sender = ((Child_Thread_Arg*)child_thread_arg)->msg_sender;

    QPID_Sender * sender = msg_sender->sender;
    QPID_Message * msg = msg_sender->message;

    for( int i = 0; i < mes_num; i++)
    {
        sender->send(*msg);
		//printf("thread %d  %d sent\n", ((Child_Thread_Arg*)child_thread_arg)->num_of_thread, i);
    }
}

void thread_create(Child_Thread_Arg * child_thread_arg)
{
    int temp;
    for(int i = 0; i < thread_num; i++)
    {
        if((temp = pthread_create(&thread[i], NULL, send_msg, &child_thread_arg[i])) != 0){
            printf("thread %d create fail num[%d]\n", i, temp);
            break;
        }
		printf("thread %d create success\n", i);
    }
}

void thread_wait(void)
{
    for(int i = 0; i < thread_num; i++)
    {
        if(thread[i] != 0)
        {
            pthread_join(thread[i],NULL);
        }
		printf(" wait thread %d sucess\n", i);
    }

	free(thread);
	free(child_thread_arg);
}
