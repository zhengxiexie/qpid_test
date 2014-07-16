sender: sender.cpp
	g++ -I ~/opt/qpid/include -L ~/opt/qpid/lib64 -lqpidmessaging -lqpidtypes -lpthread -g -gdwarf-2 -g3 sender.cpp -o sender
