
#include <iostream>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace zmq;

int main() {
	cout << "starting server...\n";
	context_t context(1);
	socket_t socket(context, ZMQ_REP);
	socket.setsockopt(ZMQ_IPV6, 1);
	int port = 1533;
	
	socket.bind("tcp://*:" + to_string(port));//listen on port

	while (true) {
		string read = s_recv(socket);
		cout << "SERVER: " << "recieved " << read << endl;

		s_send(socket, string("w"));
	}
}
