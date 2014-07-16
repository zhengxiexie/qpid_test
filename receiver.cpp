#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include <iostream>
#include <pthread.h>

// using namespace qpid::messaging;
using namespace std;

typedef qpid::messaging::Connection QPID_Connection;
typedef qpid::messaging::Session QPID_Session;

#define THREAD_NUM 20

pthread_t thread[THREAD_NUM];

int thread_num = 1;
int mes_num = 1;

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

    std::string broker_addr = string("127.0.0.1:5672");
    static const std::string topic = "amq.topic";

    QPID_Connection conn(broker_addr);

    try {

        conn.open();
        QPID_Session session = conn.createSession();
        qpid::messaging::Receiver recv = session.createReceiver(topic);

        qpid::messaging::Message msg;

        clock_t recv_begin = 0, recv_end = 0;
        recv_begin = clock();
        while (num_of_message-- >= 0) {
            qpid::messaging::Message msg = recv.fetch(qpid::messaging::Duration::SECOND * 100);
            session.acknowledge();
        }
        session.sync();
        recv_end = clock();

		int diff_time = recv_end - recv_begin;

        printf("Sending %d * %d msgs, took %d clicks(%f seconds)\n", thread_num, mes_num, diff_time,((float)diff_time)/CLOCKS_PER_SEC);
        printf("recv %d msgs, %lums\n", mes_num, timediff(recv_begin, recv_end));

        conn.close();
    } catch (const std::exception& ignored) {

    }

    return 0;

}

void send_msg(void)
{
    for( int i = 0; i < num_of_message; i++)
    {
        char buffer[64];
        sprintf(buffer, "%032d", i);
        msg.setContent(buffer);
        msg.setContentType("text/plain");
        sender.send(msg);
    }
}

void thread_create(void)
{
    int temp;
    memset(&thread, 0, sizeof(thread));
    for(int i = 0; i < THREAD_NUM; i++)
    {
        if((temp = pthread_create(&thread[0], NULL, send_msg, NULL)) != 0)
            printf("thread %d create fail", i);
    }
}

void thread_wait(void)
{
    for(int i = 0; i < THREAD_NUM; i++)
    {
        if(thread[i] != 0)
        {
            pthread_join(thread[i],NULL);
        }
    }
}
