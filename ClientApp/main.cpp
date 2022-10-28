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
  std::cout << "Socket connected to server.\n";

  socket.send({ 'H', 'i', '!' });

  size_t iResult = 0u;
  do {
    auto const received_data = socket.receive();
    iResult = received_data.size();
    if (iResult > 0)
      printf("Bytes received: %d\n", iResult);
    else if (iResult == 0)
      printf("Connection closed\n");
    else
      printf("recv failed: %d\n", WSAGetLastError());
  } while (iResult > 0);

  return 0;
}
