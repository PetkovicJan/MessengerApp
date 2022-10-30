#include "../Socket.h"
#include "../Messenger.h"

#include <iostream>

int main(int argc, char* argv[])
{
  SocketContext socket_context;

  // This machine's IP.
  auto const ip_str = "127.0.0.1";

  // Port number from MS documentation.
  auto const port_num_str = "27015";
  Address server_address(SocketType::Client, ip_str, port_num_str);

  ConnectedSocket socket(server_address);
  std::cout << "Socket connected to server.\n";

  MessengerApp messenger_app(socket);
  messenger_app.start();

  return 0;
}
