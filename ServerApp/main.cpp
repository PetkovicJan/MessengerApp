#include "../Socket.h"
#include "../Messenger.h"

#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
  SocketContext context;

  // This machine's IP.
  auto const ip_str = "192.168.8.1";

  // Port number from MS documentation.
  auto const port_num_str = "27015";

  Address local_address(SocketType::Server, ip_str, port_num_str);

  ListeningSocket listening_socket(local_address, 2);

  auto client_socket = listening_socket.accept();
  std::cout << "Client accepted\n";

  MessengerApp messenger_app(client_socket);
  messenger_app.start();

  return 0;
}
