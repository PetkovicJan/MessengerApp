#include <iostream>

#include "../Socket.h"

int main(int argc, char* argv[])
{
  SocketContext context;

  // This machine's IP.
  auto const ip_str = "192.168.8.1";

  // Port number from MS documentation.
  auto const port_num_str = "27015";

  Address local_address(SocketType::Server, ip_str, port_num_str);

  ListeningSocket listening_socket(local_address, 2);

  auto const client_socket = listening_socket.accept();

  auto const data = client_socket.receive();

  // Echo data back to the client.
  client_socket.send(data);

  return 0;
}
