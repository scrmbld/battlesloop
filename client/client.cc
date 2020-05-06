#include <iostream>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace zmq;

int main() {
	cout << "Hello World!\n";

	context_t context(1);
	socket_t socket(context, ZMQ_REQ);
	socket.setsockopt(ZMQ_IPV6, 1);
	string hostname = "localhost";
	int port = 1533;
	
	socket.connect("tcp://" + hostname + ":" + to_string(port));
	s_send(socket, string("h"));

	string response = s_recv(socket);
	cout << "received response from server: " << response << endl;

	return 0;
}
