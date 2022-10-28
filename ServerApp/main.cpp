#include <iostream>

#include "../Socket.h"

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

  auto const client_socket = listening_socket.accept();
  std::cout << "Client accepted\n";

  // Receive until the peer shuts down the connection.
  size_t iResult = 0u;
  do {
    auto const received_data = client_socket.receive();
    iResult = received_data.size();
    if (iResult > 0) {
      printf("Bytes received: %d\n", iResult);

      // Echo data back to the client.
      client_socket.send(received_data);
      printf("Bytes sent: %d\n", iResult);
    }
    else if (iResult == 0)
      printf("Connection closing...\n");
    else {
      return 1;
    }

  } while (iResult > 0);
  return 0;
}
