#include "../Socket.h"

#include <iostream>

int main(int argc, char* argv[])
{
  SocketContext socket_context;

  // This machine's IP.
  auto const ip_str = "192.168.8.1";

  // Port number from MS documentation.
  auto const port_num_str = "27015";
  Address server_address(SocketType::Client, ip_str, port_num_str);

  ConnectedSocket socket(server_address);

  socket.send({ 'H', 'i', '!' });

  auto const received_data = socket.receive();

  return 0;
}
